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
		pybind11_add_module("${subdir}bindlib" "${${subdir}_SRC}")
        target_link_libraries("${subdir}bindlib" PRIVATE "${subdir}lib")
        set_target_properties(
            ${target}
            PROPERTIES
                PREFIX ""
                DEBUG_POSTFIX "_d"
                OUTPUT_NAME "${subdir}"
        )
        if(WIN32)
            set_target_properties(
                "${subdir}bindlib"
                PROPERTIES
                SUFFIX ".pyd"
            )
        endif()
		set_target_properties("${subdir}bindlib" PROPERTIES
							  FOLDER bindings)
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