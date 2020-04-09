include(macros)
SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
FOREACH(subdir ${SUBDIRS})
	file(GLOB_RECURSE "${subdir}_SRC"
	     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	     CONFIGURE_DEPENDS
		 "${subdir}/*.cpp"
		 "${subdir}/*.hpp")
	message(STATUS "app/${subdir}:")
	add_executable("app${subdir}" "${${subdir}_SRC}")
	set_target_properties("app${subdir}" PROPERTIES
	                      FOLDER applications)
	message(STATUS "\t${${subdir}_SRC}")
	if (EXISTS "${subdir}/CMakeLists.txt")
		add_subdirectory(${subdir})
	endif()
ENDFOREACH()