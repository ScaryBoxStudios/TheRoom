module Shakefile where

import Data.Maybe
import Development.Shake
import Development.Shake.FilePath
import Development.Shake.Util
import System.Console.ANSI
import System.Console.GetOpt
import Text.Read
import qualified Data.ByteString.Char8 as BS
import qualified System.Info as System

---------------------------------------------------------------------------
-- | Project properties
---------------------------------------------------------------------------
-- Project name
projName :: String
projName = "TheRoom"

-- Source directory
srcDir :: String
srcDir = "src"

-- Intermediate directory
intDir :: String
intDir = "tmp"

-- Output directory
binDir :: String
binDir = "bin"

-- Additional include paths
includes :: [String]
includes = ["deps/" ++ l ++ "/include" | l <- ["EGL", "GL", "Glad", "Glfw", "Glm", "KHR", "Png", "Png++", "ZLib"]]

-- Additional library paths
libpath :: [String]
libpath = ["deps/" ++ l ++ "/lib/x86/Release" | l <- ["EGL", "GL", "Glad", "Glfw", "Glm", "KHR", "Png", "Png++", "ZLib"]]

-- Link libraries
libs :: [String]
libs = ["png", "zlib", "glfw", "glad", "opengl32", "glu32", "ole32", "gdi32", "advapi32", "user32", "shell32"]

---------------------------------------------------------------------------
-- |
---------------------------------------------------------------------------
-- | Final build result type
data BuildResult =
    Binary LinkResult -- ^ Executable or Shared Library
  | Archive           -- ^ Static library

-- | Link result type
data LinkResult =
    Executable      -- ^ Executable
  | SharedLibrary   -- ^ Shared (dynamically linked) library
  deriving (Enum, Eq, Show)

-- | Build variant type
data BuildVariant =
    Release         -- ^ Release builds
  | Debug           -- ^ Debug builds
  deriving (Enum, Eq, Show, Read)

---------------------------------------------------------------------------
-- | Toolchain
---------------------------------------------------------------------------
-- | Toolchain variant.
data ToolChainVariant =
    GCC       -- ^ GNU Compiler Collection (gcc) toolchain
  | LLVM      -- ^ Low-Level Virtual Machine (LLVM) toolchain
  | MSVC      -- ^ Microsoft Visual C++ (msvc) toolchain
  deriving (Eq, Show, Read)

---------------------------------------------------------------------------
-- | Toolchain Detail
---------------------------------------------------------------------------
-- | Holds necessary values to construct a compile command
data CompileParams = CompileParams {
    cflags :: [String]
  , defines :: [String]
  , includePaths :: [String]
  }

-- | Holds necessary values to construct a link command
data LinkParams = LinkParams {
    ldflags :: [String]
  , libPaths :: [String]
  , libraries :: [String]
  }

-- | Holds necessary values to construct a archive command
data ArchiveParams = ArchiveParams { miscArFlags :: [String] }

-- | Signature type of compile command generators
type CompileAction =
     CompileParams     -- ^ Compiler flags
  -> FilePath          -- ^ Input source file
  -> FilePath          -- ^ Output object file
  -> String

-- | Signature type of link command generators
type LinkAction =
     LinkParams        -- ^ Linker flags
  -> [FilePath]        -- ^ Input object files
  -> FilePath          -- ^ Output link product
  -> String

-- | Generates Gcc compile commands
gccCompileCommand :: CompileAction
gccCompileCommand params input output =
    unwords $ cxx ++ cflgs ++ ["-c"] ++ ["-o", output] ++ defs ++ incls ++ [input]
  where
    cxx = ["g++"]
    cflgs = cflags params
    defs = map ("-D"++) (defines params)
    incls = map ("-I" ++) (includePaths params)

-- | Generates Gcc link commands
gccLinkCommand :: LinkAction
gccLinkCommand params input output =
    unwords $ ld ++ ldflgs ++ ["-o", output] ++ libraryPaths ++ input ++ librs
  where
    ld = ["g++"]
    ldflgs = ldflags params
    libraryPaths = map ("-L" ++) (libPaths params)
    librs = map ("-l" ++) (libraries params)

-- | Generates Msvc compile commands
msvcCompileCommand :: CompileAction
msvcCompileCommand params input output =
    unwords $ cxx ++ ["/c"] ++ ["/Fo", output] ++ defs ++ incls ++ [input]
  where
    cxx = ["cl"]
    defs = map ("/D" ++) (defines params)
    incls = map ("/I" ++) (includePaths params)

-- | Generates Msvc link commands
msvcLinkCommand :: LinkAction
msvcLinkCommand params input output =
    unwords $ ld ++ ldflgs ++ ["/OUT:", output] ++ libraryPaths ++ input ++ librs
  where
    ld = ["link"]
    ldflgs = ldflags params
    libraryPaths = map ("/LIBPATH:" ++) (libPaths params)
    librs = map ("/IMPLIB:" ++) (libraries params)
    
---------------------------------------------------------------------------
-- OS
---------------------------------------------------------------------------
-- | Host operating system.
data OS =
    Linux
  | OSX
  | Windows
  deriving (Eq, Ord, Show)

-- | This host's operating system.
os :: OS
os =
  case System.os of
    "darwin"  -> OSX
    "mingw32" -> Windows
    "linux"   -> Linux
    _         -> error $ "Unknown host operating system: " ++ System.os

-- | CPU Arch type
data Arch =
    I386
  | X86_64
  deriving (Eq, Ord, Show, Read)

---------------------------------------------------------------------------
-- FileUtils
---------------------------------------------------------------------------
-- | File extension for executables.
executableExtension :: String
executableExtension =
  case os of
    Windows -> "exe"
    _       -> ""

-- | File extension for dynamic shared libraries.
sharedLibraryExtension :: String
sharedLibraryExtension =
  case os of
    Linux   -> "so"
    OSX     -> "dylib"
    Windows -> "dll"

---------------------------------------------------------------------------
-- Language
---------------------------------------------------------------------------
-- | Source language.
data Language =
    C       -- ^ Plain old C
  | Cpp     -- ^ C++
  | ObjC    -- ^ Objective-C
  | ObjCpp  -- ^ Objective-C with C++ (Apple extension)
  deriving (Enum, Eq, Show)

-- | Default mapping from file extension to source language.
defaultLanguageMap :: [(String, Language)]
defaultLanguageMap = concatMap f [
    (C, [".c"])
  , (Cpp, [".cc", ".CC", ".cpp", ".CPP", ".C", ".cxx", ".CXX"])
  , (ObjC, [".m"])
  , (ObjCpp, [".mm", ".M"])
  ]
  where f (lang, exts) = map (\ext -> (ext, lang)) exts

-- | Determine the source language of a file based on its extension.
languageOf :: FilePath -> Maybe Language
languageOf = flip lookup defaultLanguageMap . takeExtension

---------------------------------------------------------------------------
-- | Build Process
---------------------------------------------------------------------------
{-
    Stdout stdout <‐ cmd "cl ‐showincludes ‐c" [input] ["‐Fo" ++ output]
    need [ dropWhile isSpace x
    | x <‐ lines stdout
    , Just x <‐ [stripPrefix "Note: including file:" x]]

    usedHeaders src = [init x | x <‐ lines src, Just x <‐ [stripPrefix "#include \"" x]]
-}

---------------------------------------------------------------------------
-- | Build Parameters
---------------------------------------------------------------------------
-- The datatype that holds any flag information
data AdditionalFlag = BVFlag BuildVariant | AFlag Arch | TVFlag ToolChainVariant deriving Eq

-- Used by ReqArg to parse Build Variant Flag
parseBuildVariantFlag :: String -> Either String AdditionalFlag
parseBuildVariantFlag s =
    case readMaybe s :: Maybe BuildVariant of
        Nothing -> Left "Could not parse given build variant."
        Just x  -> Right (BVFlag x)

-- Used by ReqArg to parse Arch Flag
parseArchFlag :: String -> Either String AdditionalFlag
parseArchFlag s =
    case readMaybe s :: Maybe Arch of
        Nothing -> Left "Could not parse given architecture."
        Just x  -> Right (AFlag x)

-- Used by ReqArg to parse ToolChain Variant Flag
parseTVFlag :: String -> Either String AdditionalFlag
parseTVFlag s =
    case readMaybe s :: Maybe ToolChainVariant of
        Nothing -> Left "Could not parse given toolchain variant."
        Just x  -> Right (TVFlag x)

additionalFlags :: [OptDescr (Either String AdditionalFlag)]
additionalFlags =
    [ Option [] ["variant"]   (ReqArg parseBuildVariantFlag "VARIANT") "The build variant to make (Release/Debug)"
    , Option [] ["arch"]      (ReqArg parseArchFlag "ARCH")            "The target architecture to build for (i386/x86_64)"
    , Option [] ["toolchain"] (ReqArg parseTVFlag "TOOLCHAIN")         "The toolchain to use for building the target"
    ]

---------------------------------------------------------------------------
-- | Entrypoint
---------------------------------------------------------------------------
main :: IO ()
main = do
    let opts = shakeOptions { shakeFiles = "tmp/.shake"
                            , shakeOutput = const $ BS.putStr . BS.pack
                            }
    shakeArgsWith opts additionalFlags $ \flags targets -> return $ Just $ do
        if null targets then want [binDir </> projName <.> exe] else want targets

        "banner" ~> do
            -- Variant
            let defVariant = Release
            let variant = listToMaybe [v | BVFlag v <- flags]
            case variant of
                Nothing -> putNormal $ "No build variant given. Defaulting to " ++ show defVariant ++ "\n"
                Just x  -> putNormal $ "Build variant: " ++ show x ++ "\n"

            -- Arch
            let defArch = I386
            let arch = listToMaybe [v | AFlag v <- flags]
            case arch of
                Nothing -> putNormal $ "No target arch given. Defaulting to " ++ show defArch ++ "\n"
                Just x  -> putNormal $ "Target arch: " ++ show x ++ "\n"

            -- Toolchain
            let defToolchain = GCC
            let toolchain = listToMaybe [v | TVFlag v <- flags]
            case toolchain of
                Nothing -> putNormal $ "No toolchain variant given. Defaulting to " ++ show defToolchain ++ "\n"
                Just x  -> putNormal $ "Using toolchain: " ++ show x ++ "\n"

        "clean" ~> do
            putNormal "Cleaning..."
            removeFilesAfter "." ["tmp"]
            putNormal "All clean."

        binDir </> projName <.> exe %> \out -> do
            -- Initial banner
            need ["banner"]

            -- Gather the source files
            srcfiles <- getDirectoryFiles "" ["src//*.cpp"]

            -- Create the future object file list
            let objfiles = [intDir </> sf -<.> "o" | sf <- srcfiles]

            -- Set the object file dependency
            need objfiles

            -- Pretty print info about the command to be executed
            liftIO $ setSGR [SetColor Foreground Vivid Green]
            putNormal "[\240]"
            liftIO $ setSGR [SetColor Foreground Dull Yellow]
            putNormal $ " Linking " ++ out ++ "\n"
            liftIO $ setSGR [Reset]

            -- Schedule the link command
            let params = LinkParams { ldflags = ["-static", "-static-libgcc", "-static-libstdc++"], libPaths = libpath, libraries = libs }
            quietly $ cmd $ gccLinkCommand params objfiles out

        intDir </> "//*.o" %> \out -> do
            -- Set the source
            let c = dropDirectory1 $ out -<.> "cpp"

            -- Pretty print info about the command to be executed
            liftIO $ setSGR [SetColor Foreground Vivid Green]
            putNormal "[\175]"
            liftIO $ setSGR [SetColor Foreground Vivid Yellow]
            putNormal $ " Compiling " ++ c ++ "\n"
            liftIO $ setSGR [Reset]

            -- Schedule the compile command
            let params = CompileParams { cflags = ["-Wall", "-Wextra", "-std=c++11", "-O2"], defines = ["NDEBUG"], includePaths = includes }
            () <- quietly $ cmd (EchoStdout False) (EchoStderr False) $ gccCompileCommand params c out ++ " -MMD"

            -- Set up the dependency upon the generated dependency file
            let m = out -<.> "d"
            needMakefileDependencies m

