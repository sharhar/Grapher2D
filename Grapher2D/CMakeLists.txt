CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

add_definitions(${SDL2_DEFINITIONS})
include_directories(${SDL2_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR} )

file(GLOB_RECURSE GRAPHER_SRC *.cpp)
file(GLOB_RECURSE GRAPHER_H *.h)

add_executable(Grapher2D ${GRAPHER_H} ${GRAPHER_SRC})

target_link_libraries(Grapher2D SDL2)
target_link_libraries(Grapher2D SDL2main)
target_link_libraries(Grapher2D ${OPENGL_LIBRARIES})
target_link_libraries(Grapher2D LINK_PUBLIC GLUI)
target_link_libraries(Grapher2D glfw ${GLFW_LIBRARIES})
target_link_libraries(Grapher2D LINK_PUBLIC freetype)