file(GLOB_RECURSE TestSrcs ${CMAKE_CURRENT_LIST_DIR}/ *.cpp)

foreach(File ${TestSrcs})
	message(${File})
	get_filename_component(BaseName ${File} NAME_WE)
	add_executable(${BaseName} ${File})
	target_link_libraries(
		${BaseName}
		PRIVATE CPPTemplateGear::CPPTemplateGear
	)
endforeach()
