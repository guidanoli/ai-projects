include(macros)
SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
FOREACH(subdir ${SUBDIRS})
	file(GLOB_RECURSE "${subdir}_HEADERS"
	     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	     CONFIGURE_DEPENDS
		 "${subdir}/*.hpp")
	message(STATUS "include/${subdir}:")
	if ("${${subdir}_HEADERS}")
		target_sources("lib${subdir}" "${${subdir}_HEADERS}")
		target_include_directories("lib${subdir}" PUBLIC
			"${CMAKE_CURRENT_SOURCE_DIR}/include/${subdir}")
		message(STATUS "\t${${subdir}_HEADERS}")
	endif()
ENDFOREACH()