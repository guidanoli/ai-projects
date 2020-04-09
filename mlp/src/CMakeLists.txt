include(macros)
SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
FOREACH(subdir ${SUBDIRS})
	file(GLOB_RECURSE "${subdir}_SRC"
	     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	     CONFIGURE_DEPENDS
		 "${subdir}/*.cpp")
	message(STATUS "lib/${subdir}:")
	if ("${${subdir}_SRC}")
		add_library("lib${subdir}" "${${subdir}_SRC}")
		set_target_properties("lib${subdir}" PROPERTIES
							  FOLDER libraries)
		message(STATUS "\t${${subdir}_SRC}")
	endif()
	if (EXISTS "${subdir}/CMakeLists.txt")
		add_subdirectory(${subdir})
	endif()
ENDFOREACH()