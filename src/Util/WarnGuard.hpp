#ifndef _WARN_GUARD_HPP_
#define _WARN_GUARD_HPP_

#ifdef _MSC_VER
#define DO_PRAGMA(x) __pragma(x)
#else
#define DO_PRAGMA(x) _Pragma(#x)
#endif

//======================================================= 
// Clang
//======================================================= 
#define CLANG_WARN_GUARD_ON                         \
    DO_PRAGMA(clang diagnostic push)                \
    DO_PRAGMA(clang diagnostic ignored "-Wall")     \
    DO_PRAGMA(clang diagnostic ignored "-Wextra")

#define CLANG_WARN_GUARD_OFF                        \
    DO_PRAGMA(clang diagnostic pop)

//======================================================= 
// MSVC
//======================================================= 
#define MSVC_WARN_GUARD_ON                          \
    DO_PRAGMA(warning(push, 0))

#define MSVC_WARN_GUARD_OFF                         \
    DO_PRAGMA(warning(pop))

//======================================================= 
// Gcc
//======================================================= 
#define GCC_WARN_GUARD_ON                           \
    DO_PRAGMA(GCC diagnostic push)                  \
    DO_PRAGMA(GCC diagnostic ignored "-Wabi")                           \
    DO_PRAGMA(GCC diagnostic ignored "-Wabi-tag")                       \
    DO_PRAGMA(GCC diagnostic ignored "-Waddress")                       \
    DO_PRAGMA(GCC diagnostic ignored "-Waggregate-return")              \
    DO_PRAGMA(GCC diagnostic ignored "-Waggressive-loop-optimizations") \
    DO_PRAGMA(GCC diagnostic ignored "-Waliasing")                      \
    DO_PRAGMA(GCC diagnostic ignored "-Walign-commons")                 \
    DO_PRAGMA(GCC diagnostic ignored "-Wall")                           \
    DO_PRAGMA(GCC diagnostic ignored "-Wampersand")                     \
    DO_PRAGMA(GCC diagnostic ignored "-Warray-bounds")                  \
    DO_PRAGMA(GCC diagnostic ignored "-Warray-temporaries")             \
    DO_PRAGMA(GCC diagnostic ignored "-Wassign-intercept")              \
    DO_PRAGMA(GCC diagnostic ignored "-Wattributes")                    \
    DO_PRAGMA(GCC diagnostic ignored "-Wbad-function-cast")             \
    DO_PRAGMA(GCC diagnostic ignored "-Wbool-compare")                  \
    DO_PRAGMA(GCC diagnostic ignored "-Wbuiltin-macro-redefined")       \
    DO_PRAGMA(GCC diagnostic ignored "-Wc++-compat")                    \
    DO_PRAGMA(GCC diagnostic ignored "-Wc++11-compat")                  \
    DO_PRAGMA(GCC diagnostic ignored "-Wc++14-compat")                  \
    DO_PRAGMA(GCC diagnostic ignored "-Wc-binding-type")                \
    DO_PRAGMA(GCC diagnostic ignored "-Wc90-c99-compat")                \
    DO_PRAGMA(GCC diagnostic ignored "-Wc99-c11-compat")                \
    DO_PRAGMA(GCC diagnostic ignored "-Wcast-align")                    \
    DO_PRAGMA(GCC diagnostic ignored "-Wcast-qual")                     \
    DO_PRAGMA(GCC diagnostic ignored "-Wchar-subscripts")               \
    DO_PRAGMA(GCC diagnostic ignored "-Wcharacter-truncation")          \
    DO_PRAGMA(GCC diagnostic ignored "-Wchkp")                          \
    DO_PRAGMA(GCC diagnostic ignored "-Wclobbered")                     \
    DO_PRAGMA(GCC diagnostic ignored "-Wcomment")                       \
    DO_PRAGMA(GCC diagnostic ignored "-Wcompare-reals")                 \
    DO_PRAGMA(GCC diagnostic ignored "-Wconditionally-supported")       \
    DO_PRAGMA(GCC diagnostic ignored "-Wconversion")                    \
    DO_PRAGMA(GCC diagnostic ignored "-Wconversion-extra")              \
    DO_PRAGMA(GCC diagnostic ignored "-Wconversion-null")               \
    DO_PRAGMA(GCC diagnostic ignored "-Wcoverage-mismatch")             \
    DO_PRAGMA(GCC diagnostic ignored "-Wcpp")                           \
    DO_PRAGMA(GCC diagnostic ignored "-Wctor-dtor-privacy")             \
    DO_PRAGMA(GCC diagnostic ignored "-Wdate-time")                     \
    DO_PRAGMA(GCC diagnostic ignored "-Wdeclaration-after-statement")   \
    DO_PRAGMA(GCC diagnostic ignored "-Wdelete-incomplete")             \
    DO_PRAGMA(GCC diagnostic ignored "-Wdelete-non-virtual-dtor")       \
    DO_PRAGMA(GCC diagnostic ignored "-Wdeprecated")                    \
    DO_PRAGMA(GCC diagnostic ignored "-Wdeprecated-declarations")       \
    DO_PRAGMA(GCC diagnostic ignored "-Wdesignated-init")               \
    DO_PRAGMA(GCC diagnostic ignored "-Wdisabled-optimization")         \
    DO_PRAGMA(GCC diagnostic ignored "-Wdiscarded-array-qualifiers")    \
    DO_PRAGMA(GCC diagnostic ignored "-Wdiscarded-qualifiers")          \
    DO_PRAGMA(GCC diagnostic ignored "-Wdiv-by-zero")                   \
    DO_PRAGMA(GCC diagnostic ignored "-Wdouble-promotion")              \
    DO_PRAGMA(GCC diagnostic ignored "-Weffc++")                        \
    DO_PRAGMA(GCC diagnostic ignored "-Wempty-body")                    \
    DO_PRAGMA(GCC diagnostic ignored "-Wendif-labels")                  \
    DO_PRAGMA(GCC diagnostic ignored "-Wenum-compare")                  \
    DO_PRAGMA(GCC diagnostic ignored "-Wextra")                         \
    DO_PRAGMA(GCC diagnostic ignored "-Wfloat-equal")                   \
    DO_PRAGMA(GCC diagnostic ignored "-Wformat")                        \
    DO_PRAGMA(GCC diagnostic ignored "-Wformat-contains-nul")           \
    DO_PRAGMA(GCC diagnostic ignored "-Wformat-extra-args")             \
    DO_PRAGMA(GCC diagnostic ignored "-Wformat-nonliteral")             \
    DO_PRAGMA(GCC diagnostic ignored "-Wformat-security")               \
    DO_PRAGMA(GCC diagnostic ignored "-Wformat-signedness")             \
    DO_PRAGMA(GCC diagnostic ignored "-Wformat-y2k")                    \
    DO_PRAGMA(GCC diagnostic ignored "-Wformat-zero-length")            \
    DO_PRAGMA(GCC diagnostic ignored "-Wfree-nonheap-object")           \
    DO_PRAGMA(GCC diagnostic ignored "-Wfunction-elimination")          \
    DO_PRAGMA(GCC diagnostic ignored "-Wignored-qualifiers")            \
    DO_PRAGMA(GCC diagnostic ignored "-Wimplicit")                      \
    DO_PRAGMA(GCC diagnostic ignored "-Wimplicit-function-declaration") \
    DO_PRAGMA(GCC diagnostic ignored "-Wimplicit-int")                  \
    DO_PRAGMA(GCC diagnostic ignored "-Wimplicit-interface")            \
    DO_PRAGMA(GCC diagnostic ignored "-Wimplicit-procedure")            \
    DO_PRAGMA(GCC diagnostic ignored "-Wincompatible-pointer-types")    \
    DO_PRAGMA(GCC diagnostic ignored "-Winherited-variadic-ctor")       \
    DO_PRAGMA(GCC diagnostic ignored "-Winit-self")                     \
    DO_PRAGMA(GCC diagnostic ignored "-Winline")                        \
    DO_PRAGMA(GCC diagnostic ignored "-Wint-conversion")                \
    DO_PRAGMA(GCC diagnostic ignored "-Wint-to-pointer-cast")           \
    DO_PRAGMA(GCC diagnostic ignored "-Wintrinsic-shadow")              \
    DO_PRAGMA(GCC diagnostic ignored "-Wintrinsics-std")                \
    DO_PRAGMA(GCC diagnostic ignored "-Winvalid-memory-model")          \
    DO_PRAGMA(GCC diagnostic ignored "-Winvalid-offsetof")              \
    DO_PRAGMA(GCC diagnostic ignored "-Winvalid-pch")                   \
    DO_PRAGMA(GCC diagnostic ignored "-Wjump-misses-init")              \
    DO_PRAGMA(GCC diagnostic ignored "-Wlarger-than-")                  \
    DO_PRAGMA(GCC diagnostic ignored "-Wline-truncation")               \
    DO_PRAGMA(GCC diagnostic ignored "-Wliteral-suffix")                \
    DO_PRAGMA(GCC diagnostic ignored "-Wlogical-not-parentheses")       \
    DO_PRAGMA(GCC diagnostic ignored "-Wlogical-op")                    \
    DO_PRAGMA(GCC diagnostic ignored "-Wlong-long")                     \
    DO_PRAGMA(GCC diagnostic ignored "-Wmain")                          \
    DO_PRAGMA(GCC diagnostic ignored "-Wmaybe-uninitialized")           \
    DO_PRAGMA(GCC diagnostic ignored "-Wmemset-transposed-args")        \
    DO_PRAGMA(GCC diagnostic ignored "-Wmissing-braces")                \
    DO_PRAGMA(GCC diagnostic ignored "-Wmissing-declarations")          \
    DO_PRAGMA(GCC diagnostic ignored "-Wmissing-field-initializers")    \
    DO_PRAGMA(GCC diagnostic ignored "-Wmissing-include-dirs")          \
    DO_PRAGMA(GCC diagnostic ignored "-Wmissing-parameter-type")        \
    DO_PRAGMA(GCC diagnostic ignored "-Wmissing-prototypes")            \
    DO_PRAGMA(GCC diagnostic ignored "-Wmultichar")                     \
    DO_PRAGMA(GCC diagnostic ignored "-Wnarrowing")                     \
    DO_PRAGMA(GCC diagnostic ignored "-Wnested-externs")                \
    DO_PRAGMA(GCC diagnostic ignored "-Wnoexcept")                      \
    DO_PRAGMA(GCC diagnostic ignored "-Wnon-template-friend")           \
    DO_PRAGMA(GCC diagnostic ignored "-Wnon-virtual-dtor")              \
    DO_PRAGMA(GCC diagnostic ignored "-Wnonnull")                       \
    DO_PRAGMA(GCC diagnostic ignored "-Wodr")                           \
    DO_PRAGMA(GCC diagnostic ignored "-Wold-style-cast")                \
    DO_PRAGMA(GCC diagnostic ignored "-Wold-style-declaration")         \
    DO_PRAGMA(GCC diagnostic ignored "-Wold-style-definition")          \
    DO_PRAGMA(GCC diagnostic ignored "-Wopenmp-simd")                   \
    DO_PRAGMA(GCC diagnostic ignored "-Woverflow")                      \
    DO_PRAGMA(GCC diagnostic ignored "-Woverlength-strings")            \
    DO_PRAGMA(GCC diagnostic ignored "-Woverloaded-virtual")            \
    DO_PRAGMA(GCC diagnostic ignored "-Woverride-init")                 \
    DO_PRAGMA(GCC diagnostic ignored "-Wpacked")                        \
    DO_PRAGMA(GCC diagnostic ignored "-Wpacked-bitfield-compat")        \
    DO_PRAGMA(GCC diagnostic ignored "-Wpadded")                        \
    DO_PRAGMA(GCC diagnostic ignored "-Wparentheses")                   \
    DO_PRAGMA(GCC diagnostic ignored "-Wpedantic")                      \
    DO_PRAGMA(GCC diagnostic ignored "-Wpedantic-ms-format")            \
    DO_PRAGMA(GCC diagnostic ignored "-Wpmf-conversions")               \
    DO_PRAGMA(GCC diagnostic ignored "-Wpointer-arith")                 \
    DO_PRAGMA(GCC diagnostic ignored "-Wpointer-sign")                  \
    DO_PRAGMA(GCC diagnostic ignored "-Wpointer-to-int-cast")           \
    DO_PRAGMA(GCC diagnostic ignored "-Wpragmas")                       \
    DO_PRAGMA(GCC diagnostic ignored "-Wproperty-assign-default")       \
    DO_PRAGMA(GCC diagnostic ignored "-Wprotocol")                      \
    DO_PRAGMA(GCC diagnostic ignored "-Wreal-q-constant")               \
    DO_PRAGMA(GCC diagnostic ignored "-Wrealloc-lhs")                   \
    DO_PRAGMA(GCC diagnostic ignored "-Wrealloc-lhs-all")               \
    DO_PRAGMA(GCC diagnostic ignored "-Wredundant-decls")               \
    DO_PRAGMA(GCC diagnostic ignored "-Wreorder")                       \
    DO_PRAGMA(GCC diagnostic ignored "-Wreturn-local-addr")             \
    DO_PRAGMA(GCC diagnostic ignored "-Wreturn-type")                   \
    DO_PRAGMA(GCC diagnostic ignored "-Wselector")                      \
    DO_PRAGMA(GCC diagnostic ignored "-Wsequence-point")                \
    DO_PRAGMA(GCC diagnostic ignored "-Wshadow")                        \
    DO_PRAGMA(GCC diagnostic ignored "-Wshadow-ivar")                   \
    DO_PRAGMA(GCC diagnostic ignored "-Wshift-count-negative")          \
    DO_PRAGMA(GCC diagnostic ignored "-Wshift-count-overflow")          \
    DO_PRAGMA(GCC diagnostic ignored "-Wsign-compare")                  \
    DO_PRAGMA(GCC diagnostic ignored "-Wsign-promo")                    \
    DO_PRAGMA(GCC diagnostic ignored "-Wsized-deallocation")            \
    DO_PRAGMA(GCC diagnostic ignored "-Wsizeof-array-argument")         \
    DO_PRAGMA(GCC diagnostic ignored "-Wsizeof-pointer-memaccess")      \
    DO_PRAGMA(GCC diagnostic ignored "-Wstack-protector")               \
    DO_PRAGMA(GCC diagnostic ignored "-Wstrict-aliasing")               \
    DO_PRAGMA(GCC diagnostic ignored "-Wstrict-null-sentinel")          \
    DO_PRAGMA(GCC diagnostic ignored "-Wstrict-overflow")               \
    DO_PRAGMA(GCC diagnostic ignored "-Wstrict-prototypes")             \
    DO_PRAGMA(GCC diagnostic ignored "-Wstrict-selector-match")         \
    DO_PRAGMA(GCC diagnostic ignored "-Wsuggest-final-methods")         \
    DO_PRAGMA(GCC diagnostic ignored "-Wsuggest-final-types")           \
    DO_PRAGMA(GCC diagnostic ignored "-Wsuggest-override")              \
    DO_PRAGMA(GCC diagnostic ignored "-Wsurprising")                    \
    DO_PRAGMA(GCC diagnostic ignored "-Wswitch")                        \
    DO_PRAGMA(GCC diagnostic ignored "-Wswitch-bool")                   \
    DO_PRAGMA(GCC diagnostic ignored "-Wswitch-default")                \
    DO_PRAGMA(GCC diagnostic ignored "-Wswitch-enum")                   \
    DO_PRAGMA(GCC diagnostic ignored "-Wsync-nand")                     \
    DO_PRAGMA(GCC diagnostic ignored "-Wsystem-headers")                \
    DO_PRAGMA(GCC diagnostic ignored "-Wtabs")                          \
    DO_PRAGMA(GCC diagnostic ignored "-Wtarget-lifetime")               \
    DO_PRAGMA(GCC diagnostic ignored "-Wtraditional")                   \
    DO_PRAGMA(GCC diagnostic ignored "-Wtraditional-conversion")        \
    DO_PRAGMA(GCC diagnostic ignored "-Wtrampolines")                   \
    DO_PRAGMA(GCC diagnostic ignored "-Wtrigraphs")                     \
    DO_PRAGMA(GCC diagnostic ignored "-Wtype-limits")                   \
    DO_PRAGMA(GCC diagnostic ignored "-Wundeclared-selector")           \
    DO_PRAGMA(GCC diagnostic ignored "-Wundef")                         \
    DO_PRAGMA(GCC diagnostic ignored "-Wunderflow")                     \
    DO_PRAGMA(GCC diagnostic ignored "-Wuninitialized")                 \
    DO_PRAGMA(GCC diagnostic ignored "-Wunknown-pragmas")               \
    DO_PRAGMA(GCC diagnostic ignored "-Wunsafe-loop-optimizations")     \
    DO_PRAGMA(GCC diagnostic ignored "-Wunsuffixed-float-constants")    \
    DO_PRAGMA(GCC diagnostic ignored "-Wunused")                        \
    DO_PRAGMA(GCC diagnostic ignored "-Wunused-but-set-parameter")      \
    DO_PRAGMA(GCC diagnostic ignored "-Wunused-but-set-variable")       \
    DO_PRAGMA(GCC diagnostic ignored "-Wunused-dummy-argument")         \
    DO_PRAGMA(GCC diagnostic ignored "-Wunused-function")               \
    DO_PRAGMA(GCC diagnostic ignored "-Wunused-label")                  \
    DO_PRAGMA(GCC diagnostic ignored "-Wunused-local-typedefs")         \
    DO_PRAGMA(GCC diagnostic ignored "-Wunused-macros")                 \
    DO_PRAGMA(GCC diagnostic ignored "-Wunused-parameter")              \
    DO_PRAGMA(GCC diagnostic ignored "-Wunused-result")                 \
    DO_PRAGMA(GCC diagnostic ignored "-Wunused-value")                  \
    DO_PRAGMA(GCC diagnostic ignored "-Wunused-variable")               \
    DO_PRAGMA(GCC diagnostic ignored "-Wuse-without-only")              \
    DO_PRAGMA(GCC diagnostic ignored "-Wuseless-cast")                  \
    DO_PRAGMA(GCC diagnostic ignored "-Wvarargs")                       \
    DO_PRAGMA(GCC diagnostic ignored "-Wvariadic-macros")               \
    DO_PRAGMA(GCC diagnostic ignored "-Wvector-operation-performance")  \
    DO_PRAGMA(GCC diagnostic ignored "-Wvirtual-move-assign")           \
    DO_PRAGMA(GCC diagnostic ignored "-Wvla")                           \
    DO_PRAGMA(GCC diagnostic ignored "-Wvolatile-register-var")         \
    DO_PRAGMA(GCC diagnostic ignored "-Wwrite-strings")                 \
    DO_PRAGMA(GCC diagnostic ignored "-Wzero-as-null-pointer-constant") \
    DO_PRAGMA(GCC diagnostic ignored "-Wzerotrip")                      \
    DO_PRAGMA(GCC diagnostic ignored "-frequire-return-statement")

#define GCC_WARN_GUARD_OFF                          \
    DO_PRAGMA(GCC diagnostic pop)

//======================================================= 
// Unknown
//======================================================= 
#define UNKNOWN_WARN_GUARD_ON
#define UNKNOWN_WARN_GUARD_OFF

//======================================================= 
// All
//======================================================= 

#ifdef _MSC_VER
    #define WARN_GUARD_ON  MSVC_WARN_GUARD_ON
    #define WARN_GUARD_OFF MSVC_WARN_GUARD_OFF
#elif __clang__
    #define WARN_GUARD_ON  CLANG_WARN_GUARD_ON
    #define WARN_GUARD_OFF CLANG_WARN_GUARD_OFF
#elif __GNUC__
    #define WARN_GUARD_ON  GCC_WARN_GUARD_ON
    #define WARN_GUARD_OFF GCC_WARN_GUARD_OFF
#else
    #define WARN_GUARD_ON  UNKNOWN_WARN_GUARD_ON
    #define WARN_GUARD_OFF UNKNOWN_WARN_GUARD_OFF
#endif

#endif
