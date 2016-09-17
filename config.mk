PRJTYPE = Executable
DEFINES = _SCL_SECURE_NO_WARNINGS
LIBS = assetloader vorbis ogg freetype openal tiff png glfw glad assimp jpeg
ifeq ($(OS), Windows_NT)
	LIBS += zlib rpcrt4 winmm opengl32 glu32 ole32 gdi32 advapi32 user32 shell32
	DEFINES += _CRT_SECURE_NO_WARNINGS _CRT_SECURE_NO_DEPRECATE
else
	LIBS += z uuid GLU GL X11 Xcursor Xinerama Xrandr Xxf86vm Xi pthread dl
endif
