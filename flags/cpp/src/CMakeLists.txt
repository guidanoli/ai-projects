include(macros) 
SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
FOREACH(subdir ${SUBDIRS})
	file(GLOB_RECURSE "${subdir}_SRC"
	     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	     CONFIGURE_DEPENDS
		 "${subdir}/*.cpp"
         "${subdir}/*.h")
	message(STATUS "src/${subdir}/")
	if (NOT ("${${subdir}_SRC}" STREQUAL ""))
		add_library("${subdir}lib" "${${subdir}_SRC}")
        target_include_directories("${subdir}lib" PUBLIC
			"${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/")
		set_target_properties("${subdir}lib" PROPERTIES
                                      POSITION_INDEPENDENT_CODE ON
				      FOLDER libraries)
		FOREACH(SOURCE_FILE_PATH ${${subdir}_SRC})
			string(REPLACE "${subdir}/" ""
				SOURCE_FILE_NAME ${SOURCE_FILE_PATH})
			message(STATUS "\t${SOURCE_FILE_NAME}")
		ENDFOREACH()
	endif()
	file (GLOB_RECURSE "${subdir}_CMAKELIST"
		  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	      CONFIGURE_DEPENDS
		  "${subdir}/CMakeLists.txt")
	if (NOT ("${${subdir}_CMAKELIST}" STREQUAL ""))
		add_subdirectory(${subdir})
	endif()
ENDFOREACH()
