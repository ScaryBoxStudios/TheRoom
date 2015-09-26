module Shakefile where

import Data.IORef
import Development.Shake
import Development.Shake.FilePath
import Development.Shake.Util
import System.Console.ANSI
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

---------------------------------------------------------------------------
-- | Toolchain
---------------------------------------------------------------------------
-- | Toolchain variant.
data ToolChainVariant =
    GCC       -- ^ GNU Compiler Collection (gcc) toolchain
  | LLVM      -- ^ Low-Level Virtual Machine (LLVM) toolchain
  | MSVC      -- ^ Microsoft Visual C++ (msvc) toolchain
  deriving (Eq, Show)

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
-- | Entrypoint
---------------------------------------------------------------------------
main :: IO ()
main = do
    pRef <- newIORef $ return mempty

    shakeArgs shakeOptions { shakeFiles = "tmp/.shake"
                           , shakeOutput = const $ BS.putStr . BS.pack
                           , shakeProgress = writeIORef pRef
                           } $ do
        want [binDir </> projName <.> exe]

        "clean" ~> do
            putNormal "Cleaning..."
            removeFilesAfter "." ["tmp"]
            putNormal "All clean."

        binDir </> projName <.> exe %> \out -> do
            -- Initial banner
            let variant = "ReleaseDummy"
            let arch = "x86 Dummy"
            putNormal $ "Building " ++ variant ++ " variant for " ++ arch ++ " architecture" ++ "\n"

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

