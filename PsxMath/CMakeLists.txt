file(GLOB_RECURSE PSXMATH_SRCS 
	"*.c"
)

add_library(psxmath_static STATIC ${PSXMATH_SRCS})
set_target_properties(psxmath_static PROPERTIES OUTPUT_NAME "psxmath")
target_include_directories(psxmath_static PUBLIC "")