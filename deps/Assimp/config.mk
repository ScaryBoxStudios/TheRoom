PRJTYPE = StaticLib
SRC = $(call rwildcard, src, *.cpp) \
	  $(call rwildcard, src, *.cc) \
	  $(call rwildcard, src, *.c) \
	  $(call rwildcard, contrib, *.cpp) \
	  $(call rwildcard, contrib, *.cc) \
	  $(call rwildcard, contrib, *.c)
DEFINES = ASSIMP_BUILD_NO_OWN_ZLIB ASSIMP_BUILD_BOOST_WORKAROUND ASSIMP_BUILD_NO_C4D_IMPORTER ASSIMP_BUILD_NO_OPENGEX_IMPORTER ASSIMP_BUILD_NO_LWO_IMPORTER ASSIMP_BUILD_NO_LWS_IMPORTER
ADDINCS = ../ZLib/include contrib src/BoostWorkaround
