include(macros)
SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
FOREACH(subdir ${SUBDIRS})
	file(GLOB_RECURSE "${subdir}_SRC"
	     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	     CONFIGURE_DEPENDS
		 "${subdir}/*.cpp"
		 "${subdir}/*.hpp")
	message(STATUS "tests/${subdir}:")
	if ("${${subdir}_SRC}")
		add_executable("t${subdir}" "${${subdir}_SRC}")
		add_test(NAME ${subdir} COMMAND "t${subdir}")
		set_target_properties("t${subdir}" PROPERTIES
							  FOLDER tests)
		message(STATUS "\t${${subdir}_SRC}")
	endif()
	if (EXISTS "${subdir}/CMakeLists.txt")
		add_subdirectory(${subdir})
	endif()
ENDFOREACH()