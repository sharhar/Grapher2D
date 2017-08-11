CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

file(GLOB_RECURSE GRAPHER_SRC *.c)
file(GLOB_RECURSE GRAPHER_H *.h)

include_directories(
	"${CMAKE_SOURCE_DIR}/src"
)

if(APPLE)
	add_executable(
		Grapher2D 
		MACOSX_BUNDLE 
		${GRAPHER_H}
		${GRAPHER_SRC}
		)
else()
	add_executable(Grapher2D ${GRAPHER_H} ${GRAPHER_SRC})
endif()

if(APPLE)
	find_path(COCOA_INCLUDE_DIR Cocoa/Cocoa)
	find_library(COCOA_LIBRARY Cocoa)

	target_link_libraries (Grapher2D ${COCOA_LIBRARY})
endif()

target_link_libraries (Grapher2D LINK_PUBLIC SWin)
