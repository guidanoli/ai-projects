include(macros)
SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
FOREACH(subdir ${SUBDIRS})
	file(GLOB_RECURSE "${subdir}_SRC"
	     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	     CONFIGURE_DEPENDS
		 "${subdir}/*.cpp")
	message(STATUS "src/${subdir}/")
	if (NOT ("${${subdir}_SRC}" STREQUAL ""))
		add_library("${subdir}lib" "${${subdir}_SRC}")
		set_target_properties("${subdir}lib" PROPERTIES
							  FOLDER libraries)
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