file(GLOB_RECURSE MRBP_SRCS 
	"*.cpp"
)

add_library(mrbp_static STATIC ${MRBP_SRCS})
set_target_properties(mrbp_static PROPERTIES OUTPUT_NAME "mrbp")
target_include_directories(mrbp_static PUBLIC "")