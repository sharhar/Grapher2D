CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

file(GLOB_RECURSE GRAPHER_SRC *.cpp)
file(GLOB_RECURSE GRAPHER_H *.h)

if(APPLE)
	add_executable(
		Grapher2D 
		MACOSX_BUNDLE 
		${GRAPHER_H}
		${GRAPHER_SRC}
		"${CMAKE_SOURCE_DIR}/src/arial.ttf"
		)
	
	SET_SOURCE_FILES_PROPERTIES(
	  "${CMAKE_SOURCE_DIR}/src/arial.ttf"
	  PROPERTIES
	  MACOSX_PACKAGE_LOCATION Resources
	  )
elseif(MSVC)
	add_executable(Grapher2D ${GRAPHER_H} ${GRAPHER_SRC})
	file(COPY "${CMAKE_SOURCE_DIR}/src/arial.ttf" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug")
	file(COPY "${CMAKE_SOURCE_DIR}/src/arial.ttf" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release")
	file(COPY "${CMAKE_SOURCE_DIR}/src/arial.ttf" DESTINATION "${CMAKE_BINARY_DIR}/src")
else()
	add_executable(Grapher2D ${GRAPHER_H} ${GRAPHER_SRC})
	file(COPY "${CMAKE_SOURCE_DIR}/src/arial.ttf" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

target_link_libraries (Grapher2D LINK_PUBLIC ${OPENGL_LIBRARIES})
target_link_libraries (Grapher2D LINK_PUBLIC GLUI)
target_link_libraries (Grapher2D glfw ${GLFW_LIBRARIES})
target_link_libraries (Grapher2D LINK_PUBLIC freetype)
