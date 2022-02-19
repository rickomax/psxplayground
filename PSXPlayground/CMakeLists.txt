cmake_minimum_required (VERSION 3.8)
file(GLOB_RECURSE PSXPLAYGROUND_SRCS 
	"*.c"
)
set_source_files_properties( ${PSXPLAYGROUND_SRCS} PROPERTIES LANGUAGE C)
add_executable (PSXPlayground ${PSXPLAYGROUND_SRCS} "Include/FileUtils.h")
add_custom_command(
	TARGET PSXPlayground PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory 
	${CMAKE_SOURCE_DIR}/SampleProject $<TARGET_FILE_DIR:PSXPlayground>/SampleProject
)
target_link_libraries(PSXPlayground psxmath_static)
target_link_libraries(PSXPlayground lua_static)
target_link_libraries(PSXPlayground psycross_static)
target_link_libraries(PSXPlayground mrbp_static)