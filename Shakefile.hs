module Shakefile where

import Control.Monad
import Data.Char
import Data.List
import Data.Maybe
import Development.Shake
import Development.Shake.FilePath
import System.Console.ANSI
import System.Console.GetOpt
import System.Directory
import Text.Read
import qualified Data.ByteString.Char8 as BS
import qualified System.Info as System

---------------------------------------------------------------------------
-- | Project properties
---------------------------------------------------------------------------
-- Project type
projType :: BuildResult
projType = Binary Executable

-- Project name
projName :: String
projName = "TheRoom"

-- Source directory
srcDir :: String
srcDir = "src"

-- Build directory
bldDir :: String
bldDir = "tmp"

-- Output directory for executables
binDir :: String
binDir = "bin"

-- Output directory for libraries
libDir :: String
libDir = "lib"

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
-- | Project Target
---------------------------------------------------------------------------
-- | Final build result type
data BuildResult =
    Binary LinkResult -- ^ Executable or Shared Library
  | Archive           -- ^ Static library
  deriving (Eq, Show)

-- | Link result type
data LinkResult =
    Executable      -- ^ Executable
  | SharedLibrary   -- ^ Shared (dynamically linked) library
  deriving (Eq, Show)

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
    unwords $ cxx ++ ["/c"] ++ ["/Fo" ++ output] ++ defs ++ incls ++ [input]
  where
    cxx = ["cl"]
    defs = map ("/D" ++) (defines params)
    incls = map ("/I" ++) (includePaths params)

-- | Generates Msvc link commands
msvcLinkCommand :: LinkAction
msvcLinkCommand params input output =
    unwords $ ld ++ ldflgs ++ ["/OUT:" ++ output] ++ libraryPaths ++ input ++ librs
  where
    ld = ["link"]
    ldflgs = ldflags params
    libraryPaths = map ("/LIBPATH:" ++) (libPaths params)
    librs = map (++ ".lib") (libraries params)
    
---------------------------------------------------------------------------
-- | Toolchain Default flags
---------------------------------------------------------------------------
-- Msvc
msvcDefaultCompilerFlags :: BuildVariant -> [String]
msvcDefaultCompilerFlags variant =
    ["/nologo", "/EHsc", "/W4"] ++
    case variant of
      Release -> ["/MT", "/O2"]
      Debug   -> ["/MTd", "/Zi", "/Od", "/FS"]

msvcDefaultLinkerFlags :: BuildVariant -> [String]
msvcDefaultLinkerFlags variant =
    ["/nologo", "/manifest", "/entry:mainCRTStartup"] ++
    case variant of
        Release -> ["/incremental:NO"]
        Debug   -> ["/debug"]

-- Gcc
gccDefaultCompilerFlags :: BuildVariant -> [String]
gccDefaultCompilerFlags variant =
    ["-Wall", "-Wextra", "-std=c++11"] ++
    case variant of
        Release -> ["-O2"]
        Debug   -> ["-g", "-O0"]

gccDefaultLinkerFlags :: BuildVariant -> [String]
gccDefaultLinkerFlags variant =
    ["-static", "-static-libgcc", "-static-libstdc++"] ++
    case variant of
        Release -> []
        Debug   -> []

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

-- | File prefix for static libraries.
staticLibPrefix :: ToolChainVariant -> String
staticLibPrefix t =
  case t of
    MSVC -> ""
    _    -> "lib"

-- | File extension for static libraries.
staticLibExtension :: ToolChainVariant -> String
staticLibExtension t =
    if os == Windows && t == MSVC
        then "lib"
        else "a"

-- | Master build output name construction.
masterOutName :: BuildResult -> ToolChainVariant -> String -> String
masterOutName b t n = prefix ++ name <.> extension
    where
        prefix    = case b of
                      Binary _ -> ""
                      Archive  -> staticLibPrefix t
        name      = case b of
                      Binary _ -> n
                      Archive  -> case t of
                                     MSVC -> n
                                     _    -> map toLower n
        extension = case b of
                         Binary Executable    -> executableExtension
                         Binary SharedLibrary -> sharedLibraryExtension
                         Archive -> staticLibExtension t

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
-- | Include parser
---------------------------------------------------------------------------
-- | Strips the filename from an include preprocessor command
takeIncludePath :: String -> String
takeIncludePath = tail . init . (!! 1) . words

-- | Shows if given line is an include preprocessor line with double quotes
isIncludeLine :: String -> Bool
isIncludeLine line = all ($ line)
    [ isPrefixOf "#include" . dropWhile isSpace
    , (== '"') . head . (!! 1) . words
    ]

-- | Lists the include entries in the given file contents
listIncludes :: String -> [FilePath]
listIncludes contents = map takeIncludePath . filter isIncludeLine $ lines contents

-- | Gathers a list with the headers that the given source file depends on
gatherHeaderDeps :: [FilePath] -> FilePath -> IO [FilePath]
gatherHeaderDeps searchDirs src = do
    let gatherInternal sDirs chkdFiles input = do
        filepath <- findFile searchDirs input
        case filepath of
          Nothing -> return []
          Just f  -> if f `elem` chkdFiles
                        then return []
                        else do
                               contents <- readFile f
                               let incls = listIncludes contents
                               moreIncs <- mapM (gatherInternal sDirs (f : chkdFiles)) incls
                               return (f : concat moreIncs)
    liftM (\l -> if null l then [] else tail l) (gatherInternal searchDirs [] src)

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
-- | Command builders
---------------------------------------------------------------------------
-- Compile command builder
genCompileCmd :: ToolChainVariant -> BuildVariant -> FilePath -> FilePath -> String
genCompileCmd toolchain variant =
    ccGen params
  where
    ccGen = case toolchain of
              MSVC -> msvcCompileCommand
              GCC  -> gccCompileCommand
              LLVM -> gccCompileCommand
    params = CompileParams
             { cflags = (case toolchain of
                           MSVC -> msvcDefaultCompilerFlags
                           GCC  -> gccDefaultCompilerFlags
                           LLVM -> gccDefaultCompilerFlags) variant
             , defines = []
             , includePaths = includes
             }

-- Link command builder
genLinkCmd :: ToolChainVariant -> BuildVariant -> [FilePath] -> FilePath -> String
genLinkCmd toolchain variant =
    linkGen params
  where
    linkGen = case toolchain of
                MSVC -> msvcLinkCommand
                GCC  -> gccLinkCommand
                LLVM -> gccLinkCommand
    params = LinkParams
             { ldflags = (case toolchain of
                            MSVC -> msvcDefaultLinkerFlags
                            GCC  -> gccDefaultLinkerFlags
                            LLVM -> gccDefaultLinkerFlags) variant
             , libPaths = libpath
             , libraries = libs
             }

---------------------------------------------------------------------------
-- | Entrypoint
---------------------------------------------------------------------------
main :: IO ()
main = do
    let opts = shakeOptions { shakeFiles = bldDir </> ".shake"
                            , shakeOutput = const $ BS.putStr . BS.pack
                            }
    shakeArgsWith opts additionalFlags $ \flags targets -> return $ Just $ do
        -- Extract the parameters from the flag arguments or set defaults if not given
        let defVariant = Release
        let defArch = I386
        let defToolchain = GCC

        let givenVariant = listToMaybe [v | BVFlag v <- flags]
        let givenArch = listToMaybe [v | AFlag v <- flags]
        let givenToolchain = listToMaybe [v | TVFlag v <- flags]

        let variant = fromMaybe defVariant givenVariant
        let arch = fromMaybe defArch givenArch
        let toolchain = fromMaybe defToolchain givenToolchain

        -- Set the main target
        let outName = masterOutName projType toolchain projName

        let mainTgt = (case projType of
                        Binary _ -> binDir
                        Archive  -> libDir) </> outName

        if null targets then want [mainTgt] else want targets

        -- Shows info about the build that follows
        "banner" ~> do
            -- Variant
            putNormal $ (case givenVariant of
                            Nothing -> "No build variant given. Defaulting to: "
                            Just _  -> "Build variant: ") ++ show variant ++ "\n"
            -- Arch
            putNormal $ (case givenArch of
                            Nothing -> "No target arch given. Defaulting to: "
                            Just _  -> "Target arch: ") ++ show arch ++ "\n"
            -- Toolchain
            putNormal $ (case givenToolchain of
                            Nothing -> "No toolchain variant given. Defaulting to: "
                            Just _  -> "Using toolchain: ") ++ show toolchain ++ "\n"

        "clean" ~> do
            putNormal "Cleaning...\n"
            removeFilesAfter "." [bldDir]
            putNormal "All clean.\n"

        mainTgt %> \out -> do
            -- Initial banner
            need ["banner"]

            -- Gather the source files
            srcfiles <- getDirectoryFiles srcDir ["//*.cpp"]

            -- Create the future object file list
            let objfiles = [bldDir </> sf <.> "o" | sf <- srcfiles]

            -- Set the object file dependency
            need objfiles

            -- Pretty print info about the command to be executed
            liftIO $ setSGR [SetColor Foreground Dull Green]
            putNormal "[\240] Linking "
            liftIO $ setSGR [SetColor Foreground Dull Yellow]
            putNormal $ out ++ "\n"
            liftIO $ setSGR [Reset]

            -- Schedule the link command
            quietly $ cmd $ genLinkCmd toolchain variant objfiles out

        bldDir <//> "*.o" %> \out -> do
            -- Set the source
            let c = toStandard srcDir </> dropDirectory1 (dropExtension out)
            let cdir = toStandard $ srcDir </> dropDirectory1 (takeDirectory out)

            -- Pretty print info about the command to be executed
            liftIO $ setSGR [SetColor Foreground Vivid Green]
            putNormal "[\175] Compiling "
            liftIO $ setSGR [SetColor Foreground Vivid Yellow]
            putNormal $ c ++ "\n"
            liftIO $ setSGR [Reset]

            -- Schedule the compile command
            () <- quietly $ cmd (EchoStdout False) (EchoStderr False) $ genCompileCmd toolchain variant c out

            -- Set up the dependencies upon the header files
            let fileName = dropExtension (takeFileName out)
            headerDeps <- liftIO $ gatherHeaderDeps (cdir : includes) fileName
            let prettyHeaderDeps = [normaliseEx x | x <- headerDeps]
            --putNormal $ "DEBUG: Deps for " ++ out ++ " are: " ++ show prettyHeaderDeps ++ "\n"
            need prettyHeaderDeps

