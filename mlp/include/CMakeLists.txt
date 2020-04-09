include(macros)
SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
FOREACH(subdir ${SUBDIRS})
	file(GLOB_RECURSE "${subdir}_HEADERS"
	     CONFIGURE_DEPENDS
		 "${subdir}/*.h")
	message(STATUS "include/${subdir}/")
	if (NOT ("${${subdir}_HEADERS}" STREQUAL ""))
		target_sources("${subdir}lib" PUBLIC "${${subdir}_HEADERS}")
		target_include_directories("${subdir}lib" PUBLIC
			"${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/")
		string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/" ""
			HEADER_FILES ${${subdir}_HEADERS})
		FOREACH(HEADER_FILE ${HEADER_FILES})
			message(STATUS "\t${HEADER_FILE}")
		ENDFOREACH()
	endif()
ENDFOREACH()