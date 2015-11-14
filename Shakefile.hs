module Shakefile where

import Control.Concurrent.MVar
import Control.Monad
import Data.Char
import Data.List
import Data.List.Split
import Data.Maybe
import Development.Shake
import Development.Shake.FilePath
import System.Console.ANSI
import System.Console.GetOpt
import System.Directory
import Text.Read
import qualified Data.ByteString.Char8 as BS
import qualified System.Info as System

--
-- Precompile for better performance with:
--   ghc --make Shakefile -main-is Shakefile -threaded -rtsopts -with-rtsopts="-I0 -qg -qb" -o sbuild
--

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

-- Link libraries
libs :: [String]
libs = ["png", "glfw", "glad"] ++
        case hostOs of
            Windows -> ["zlib", "opengl32", "glu32", "ole32", "gdi32", "advapi32", "user32", "shell32"]
            Linux   -> ["z", "GLU"]
            _       -> []

-- Defines
defines :: [String]
defines = []

-- Default build variant if not specified
defVariant :: BuildVariant
defVariant = Release

-- Default toolchain if not specified
defToolchain :: ToolChainVariant
defToolchain = GCC

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
  , additionalDefines :: [String]
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

-- | Signature type of archive command generators
type ArchiveAction =
     ArchiveParams
  -> [FilePath]        -- ^ Input object files
  -> FilePath          -- ^ Output archive
  -> String

-- | Generates Gcc compile commands
gccCompileCommand :: CompileAction
gccCompileCommand params input output =
    unwords $ cxx ++ cflgs ++ ["-c"] ++ ["-o", output] ++ defs ++ incls ++ [input]
  where
    cxx = ["g++"]
    cflgs = cflags params
    defs = map ("-D"++) (additionalDefines params)
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

-- | Generates Gcc archive commands
gccArchiveCommand :: ArchiveAction
gccArchiveCommand params input output =
    unwords $ ar ++ arflgs ++ miscflgs ++ [output] ++ input
  where
    ar = ["ar"]
    arflgs = ["rcs"]
    miscflgs = miscArFlags params

-- | Generates Msvc compile commands
msvcCompileCommand :: CompileAction
msvcCompileCommand params input output =
    unwords $ cxx ++ cflgs ++ ["/c"] ++ ["/Fo" ++ output] ++ defs ++ incls ++ [input]
                  ++ ["/Fd" ++ foldr1 (</>) (take 4 (splitDirectories output)) ++ "/" ]
  where
    cxx = ["cl"]
    cflgs = cflags params
    defs = map ("/D" ++) (additionalDefines params)
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
    
-- | Generates Msvc archive commands
msvcArchiveCommand :: ArchiveAction
msvcArchiveCommand params input output =
    unwords $ ar ++ arflgs ++ miscflgs ++ ["/OUT:" ++ output] ++ input
  where
    ar = ["lib"]
    arflgs = []
    miscflgs = miscArFlags params

-- | Generates Clang compile commands
clangCompileCommand :: CompileAction
clangCompileCommand params input output =
    unwords $ (["clang++"] ++ ) . tail . words $ gccCompileCommand params input output

-- | Generates Clang link commands
clangLinkCommand :: LinkAction
clangLinkCommand params input output =
    unwords $ (["clang++"] ++ ) . tail . words $ gccLinkCommand params input output

-- | Generates Clang archive commands
clangArchiveCommand :: ArchiveAction
clangArchiveCommand = gccArchiveCommand

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

msvcDefaultLinkerFlags :: LinkResult -> BuildVariant -> [String]
msvcDefaultLinkerFlags linkType variant =
    ["/nologo", "/manifest"] ++
    (case linkType of
         Executable    -> ["/entry:mainCRTStartup"]
         SharedLibrary -> ["/DLL"]) ++
    case variant of
        Release -> ["/incremental:NO"]
        Debug   -> ["/debug"]

msvcDefaultArchiverFlags :: [String]
msvcDefaultArchiverFlags = ["/nologo"]

-- Gcc
gccDefaultCompilerFlags :: BuildVariant -> [String]
gccDefaultCompilerFlags variant =
    ["-Wall", "-Wextra", "-std=c++11"] ++
    case variant of
        Release -> ["-O2"]
        Debug   -> ["-g", "-O0"]

gccDefaultLinkerFlags :: OS -> LinkResult -> BuildVariant -> [String]
gccDefaultLinkerFlags os linkType variant =
    (case os of
        Windows -> ["-static", "-static-libgcc", "-static-libstdc++"]
        Linux   -> []
        OSX     -> []) ++
    (case linkType of
        Executable    -> []
        SharedLibrary -> []) ++
    case variant of
        Release -> []
        Debug   -> []

gccDefaultArchiverFlags :: [String]
gccDefaultArchiverFlags = []

-- Clang
clangDefaultCompilerFlags :: BuildVariant -> [String]
clangDefaultCompilerFlags = gccDefaultCompilerFlags

clangDefaultLinkerFlags :: OS -> LinkResult -> BuildVariant -> [String]
clangDefaultLinkerFlags = gccDefaultLinkerFlags

clangDefaultArchiverFlags :: [String]
clangDefaultArchiverFlags = gccDefaultArchiverFlags

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
hostOs :: OS
hostOs =
  case System.os of
    "darwin"  -> OSX
    "mingw32" -> Windows
    "linux"   -> Linux
    _         -> error $ "Unknown host operating system: " ++ System.os

-- | CPU Arch type
data Arch =
    X86
  | X64
  deriving (Eq, Ord, Show, Read)

prettyShowArch :: Arch -> String
prettyShowArch a = map toLower (show a)

---------------------------------------------------------------------------
-- FileUtils
---------------------------------------------------------------------------
-- | File extension for executables.
executableExtension :: String
executableExtension =
  case hostOs of
    Windows -> "exe"
    _       -> ""

-- | File extension for dynamic shared libraries.
sharedLibraryExtension :: String
sharedLibraryExtension =
  case hostOs of
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
    if hostOs == Windows && t == MSVC
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

-- Used by ReqArg to parse ToolChain Variant Flag
parseTVFlag :: String -> Either String AdditionalFlag
parseTVFlag s =
    case readMaybe s :: Maybe ToolChainVariant of
        Nothing -> Left "Could not parse given toolchain variant."
        Just x  -> Right (TVFlag x)

additionalFlags :: [OptDescr (Either String AdditionalFlag)]
additionalFlags =
    [ Option [] ["variant"]   (ReqArg parseBuildVariantFlag "VARIANT") "The build variant to make (Release/Debug)"
    , Option [] ["toolchain"] (ReqArg parseTVFlag "TOOLCHAIN")         "The toolchain to use for building the target"
    ]

---------------------------------------------------------------------------
-- | Command builders
---------------------------------------------------------------------------
-- Compile command builder
genCompileCmd :: ToolChainVariant -> BuildVariant -> [FilePath] -> [FilePath] -> FilePath -> FilePath -> String
genCompileCmd toolchain variant includes defs =
    ccGen params
  where
    ccGen = case toolchain of
              MSVC -> msvcCompileCommand
              GCC  -> gccCompileCommand
              LLVM -> clangCompileCommand
    params = CompileParams
             { cflags = (case toolchain of
                           MSVC -> msvcDefaultCompilerFlags
                           GCC  -> gccDefaultCompilerFlags
                           LLVM -> clangDefaultCompilerFlags) variant
             , additionalDefines = defs
             , includePaths = includes
             }

-- Link command builder
genLinkCmd :: OS -> LinkResult -> ToolChainVariant -> BuildVariant -> [FilePath] -> [FilePath] -> FilePath -> String
genLinkCmd os linkType toolchain variant libpaths =
    linkGen params
  where
    linkGen = case toolchain of
                MSVC -> msvcLinkCommand
                GCC  -> gccLinkCommand
                LLVM -> clangLinkCommand
    params = LinkParams
             { ldflags = case toolchain of
                            MSVC -> msvcDefaultLinkerFlags linkType variant
                            GCC  -> gccDefaultLinkerFlags os linkType variant
                            LLVM -> clangDefaultLinkerFlags os linkType variant
             , libPaths = libpaths
             , libraries = libs
             }

-- Archive command builder
genArchiveCmd :: ToolChainVariant -> [FilePath] -> FilePath -> String
genArchiveCmd toolchain =
    (case toolchain of
          MSVC -> msvcArchiveCommand
          GCC  -> gccArchiveCommand
          LLVM -> clangArchiveCommand)
        ArchiveParams { miscArFlags = case toolchain of
                           MSVC -> msvcDefaultArchiverFlags
                           GCC  -> gccDefaultArchiverFlags
                           LLVM -> clangDefaultArchiverFlags }

---------------------------------------------------------------------------
-- | Enviroment Probers
---------------------------------------------------------------------------
-- Converts the triplet gained by a command such as 'gcc -dumpmachine'
-- to an target Arch datatype
gccTripletToArch :: String -> Maybe Arch
gccTripletToArch out =
    case machine of
        "i386"   -> Just X86
        "i486"   -> Just X86
        "i586"   -> Just X86
        "i686"   -> Just X86
        "x86_64" -> Just X64
        _        -> Nothing
  where
      machine = head $ splitOn "-" $ head $ lines out

-- Converts the info gained by the 'cl' command to an Arch datatype
msvcClInfoToArch :: String -> Maybe Arch
msvcClInfoToArch out =
    case machine of
        "x86" -> Just X86
        "x64" -> Just X64
        _     -> Nothing
  where
      machine = last $ words $ head $ lines out

clangTripletToArch :: String -> Maybe Arch
clangTripletToArch = gccTripletToArch

gatherArchFromToolchain :: ToolChainVariant -> IO (Maybe Arch)
gatherArchFromToolchain toolchain = do
    let infoCmd =
          case toolchain of
            MSVC -> "cl"
            GCC  -> "gcc -dumpmachine"
            LLVM -> "clang -dumpmachine"
    (Stdout out, Stderr err) <- cmd (EchoStdout False) (EchoStderr False) infoCmd
    return $ case toolchain of
               MSVC -> msvcClInfoToArch err
               GCC  -> gccTripletToArch out
               LLVM -> clangTripletToArch out

---------------------------------------------------------------------------
-- | Entrypoint
---------------------------------------------------------------------------
main :: IO ()
main = do
    -- The mutex for the status messages
    stdoutMvar <- newMVar ()
    let opts = shakeOptions { shakeFiles = bldDir </> ".shake"
                            , shakeOutput = const $ BS.putStr . BS.pack
                            , shakeThreads = 0
                            }
    shakeArgsWith opts additionalFlags $ \flags targets -> do
      -- Extract the parameters from the flag arguments or set defaults if not given
      let givenVariant = listToMaybe [v | BVFlag v <- flags]
      let givenToolchain = listToMaybe [v | TVFlag v <- flags]

      let variant = fromMaybe defVariant givenVariant
      let toolchain = fromMaybe defToolchain givenToolchain

      -- Gather the target architecture from the used compiler
      foundArch <- gatherArchFromToolchain toolchain
      let arch = fromMaybe X86 foundArch

      return $ Just $ do
        -- Set the main target
        let outName = masterOutName projType toolchain projName
        let mainTgt = (case projType of
                        Binary _ -> "bin"
                        Archive  -> "lib")
                      </> prettyShowArch arch
                      </> show variant
                      </> outName
        if null targets then want [mainTgt] else want targets

        -- Set the build directory for the current run
        let buildDir = bldDir </> show toolchain </> prettyShowArch arch </> show variant

        -- Shows info about the build that follows
        "banner" ~> do
            -- Arch
            putNormal $ (case foundArch of
                            Nothing -> "Could not detect target Architecture. Defaulting to: "
                            Just _  -> "Target Arch: ") ++ prettyShowArch arch ++ "\n"
            -- Variant
            putNormal $ (case givenVariant of
                            Nothing -> "No build variant given. Defaulting to: "
                            Just _  -> "Build variant: ") ++ show variant ++ "\n"
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
            srcfiles <- getDirectoryFiles srcDir ["//*.cpp", "//*.cc", "//*.c"]

            -- Create the future object file list
            let objfiles = [buildDir </> sf <.> "o" | sf <- srcfiles]

            -- Set the object file dependency
            need objfiles

            -- Gather additional library paths
            let depsFolder = "deps"
            depsFolderExists <- Development.Shake.doesDirectoryExist depsFolder
            libpaths <- if depsFolderExists
                          then do
                            deps <- Development.Shake.getDirectoryContents depsFolder
                            return [depsFolder </> l </> "lib" </> prettyShowArch arch </> show variant | l <- deps]
                          else
                            return []

            -- Construct the main output command
            let outCommand =
                 case projType of
                   Binary lr ->
                       -- Link command
                       genLinkCmd hostOs lr toolchain variant libpaths objfiles out
                   Archive   ->
                       -- Archive command
                       genArchiveCmd toolchain objfiles out

            -- Pretty print info about the command to be executed
            liftIO $ setSGR [SetColor Foreground Dull Green]
            putNormal "[\240] Linking "
            liftIO $ setSGR [SetColor Foreground Dull Yellow]
            putNormal $ out ++ "\n"
            liftIO $ setSGR [Reset]

            -- Print additional info on verbose builds
            verbosity <- getVerbosity
            when (verbosity >= Loud) $
                putNormal $ "Executing command: " ++ outCommand ++ "\n"

            -- Execute main output command
            quietly $ cmd outCommand :: Action ()

            -- Copy pdb on MSVC debug builds
            pdb <- liftM head $ getDirectoryFiles buildDir ["*.pdb"]
            when (toolchain == MSVC && variant == Debug) $
                copyFile' (buildDir </> pdb) (takeDirectory mainTgt </> pdb)

        buildDir <//> "*.o" %> \out -> do
            -- Set the source
            let dropDirectory n = foldr (.) id (replicate n dropDirectory1)
            let c = toStandard $ srcDir </> dropDirectory 4 (dropExtension out)
            let cdir = toStandard $ srcDir </> dropDirectory 4 (takeDirectory out)

            -- Gather additional include paths
            deps <- Development.Shake.getDirectoryContents "deps"
            let includes = "include" : ["deps" </> l </> "include" | l <- deps]

            -- Construct the command to be executed
            let compileCmd = genCompileCmd toolchain variant includes defines c out

            -- Pretty print info about the command to be executed
            verbosity <- getVerbosity

            liftIO $ takeMVar stdoutMvar
            liftIO $ setSGR [SetColor Foreground Vivid Green]
            putNormal "[\175] Compiling "
            liftIO $ setSGR [SetColor Foreground Vivid Yellow]
            putNormal $ c ++ "\n"
            liftIO $ setSGR [Reset]
            when (verbosity >= Loud) $
                putNormal $ "Executing command: " ++ compileCmd ++ "\n"
            liftIO $ putMVar stdoutMvar ()

            -- Execute the command
            () <- quietly $ cmd (EchoStdout False) (EchoStderr True) compileCmd

            -- Set up the dependencies upon the header files
            let fileName = dropExtension (takeFileName out)
            headerDeps <- liftIO $ gatherHeaderDeps (cdir : includes) fileName
            let prettyHeaderDeps = [normaliseEx x | x <- headerDeps]
            --putNormal $ "DEBUG: Deps for " ++ out ++ " are: " ++ show prettyHeaderDeps ++ "\n"
            need prettyHeaderDeps

