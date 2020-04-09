include(macros)
SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
FOREACH(subdir ${SUBDIRS})
	file(GLOB_RECURSE "${subdir}_SRC"
	     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	     CONFIGURE_DEPENDS
		 "${subdir}/*.cpp"
		 "${subdir}/*.h")
	message(STATUS "tests/${subdir}:")
	if (NOT ("${${subdir}_SRC}" STREQUAL ""))
		add_executable("${subdir}test" "${${subdir}_SRC}")
		target_link_libraries("${subdir}test" "${subdir}lib")
		add_test(NAME ${subdir} COMMAND "${subdir}test")
		set_target_properties("${subdir}test" PROPERTIES
							  FOLDER tests)
		string(REPLACE "${subdir}/" ""
			SOURCE_FILES ${${subdir}_SRC})
		FOREACH(SOURCE_FILE ${SOURCE_FILES})
			message(STATUS "\t${SOURCE_FILE}")
		ENDFOREACH()
	endif()
	if (EXISTS "${subdir}/CMakeLists.txt")
		add_subdirectory(${subdir})
	endif()
ENDFOREACH()