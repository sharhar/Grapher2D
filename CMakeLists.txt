CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set(EXT_EXTRACT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext")

function(download_library name url)
	if(NOT EXISTS "${EXT_EXTRACT_DIR}/${name}")
		if(NOT EXISTS "${EXT_EXTRACT_DIR}/${name}.tar.gz")
			message(STATUS "Downloading ${EXT_EXTRACT_DIR}/${name} from ${url}")
			file(DOWNLOAD "${url}" ${EXT_EXTRACT_DIR}/${name}.tar.gz TIMEOUT 10)
		endif()
		
		file(MAKE_DIRECTORY ${EXT_EXTRACT_DIR}/${name})
		
		execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${name}.tar.gz WORKING_DIRECTORY ${EXT_EXTRACT_DIR})
	endif()
	
	message(STATUS "Found ${name} in source directory")
endfunction()

PROJECT(Grapher2D)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_VERBOSE_MAKEFILE ON)

#https://iweb.dl.sourceforge.net/project/glew/glew/2.0.0/glew-2.0.0.zip


set(GLEW_SOURCE_VERSION "2.0.0")
set(GLEW_SOURCE_NAME "glew-${GLEW_SOURCE_VERSION}")
set(GLEW_URL "https://kent.dl.sourceforge.net/project/glew/glew/${GLEW_SOURCE_VERSION}/${GLEW_SOURCE_NAME}.tgz")


if(NOT EXISTS ${PROJECT_SOURCE_DIR}/${GLEW_SOURCE_NAME})
	download_library(${GLEW_SOURCE_NAME} ${GLEW_URL})
	add_subdirectory(${EXT_EXTRACT_DIR}/${GLEW_SOURCE_NAME}/build/cmake)
	
	if(BUILD_SHARED_LIBS)
		add_definitions(-DGLEW_BUILD)
	else()
		add_definitions(-DGLEW_STATIC)
	endif()
	
endif()

set(GLEW_LIBRARY glew_s)
set(GLEW_INCLUDE_DIR ${EXT_EXTRACT_DIR}/${GLEW_SOURCE_NAME}/include)

if(NOT MSVC)
	include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
	CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
	if(COMPILER_SUPPORTS_CXX11)
		message(STATUS "Found CXX11 support in ${CMAKE_CXX_COMPILER}")
	    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	elseif(COMPILER_SUPPORTS_CXX0X)
		message(STATUS "Found CXX0X support in ${CMAKE_CXX_COMPILER}")
	    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	else()
		message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
	endif()

endif()

option(GLUI_BUILD_TEST OFF)

include_directories(
	"${CMAKE_SOURCE_DIR}/GLUI/include"
)

add_subdirectory(GLUI)
add_subdirectory(src)