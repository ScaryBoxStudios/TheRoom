PRJTYPE = Executable
LIBS = glfw glad
DEFINES = _SCL_SECURE_NO_WARNINGS
LIBS = vorbis ogg freetype openal tiff png glfw glad assimp jpeg
ifeq ($(OS), Windows_NT)
	LIBS += zlib rpcrt4 winmm opengl32 glu32 ole32 gdi32 advapi32 user32 shell32
else
	LIBS += z uuid GLU GL X11 Xcursor Xinerama Xrandr Xxf86vm Xi pthread
endif