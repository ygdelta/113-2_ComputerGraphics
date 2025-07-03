cmake_minimum_required(VERSION 2.8)
project( main )

find_package( OpenCV REQUIRED )
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# set(GCC_COVERAGE_COMPILE_FLAGS "-lGL -lGLU -lglut")
include_directories( ${OpenCV_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLUT_INCLUDE_DIRS})

add_executable( main 113598017_LAB11.cpp )
target_link_libraries( main 
    ${OpenCV_LIBS} 
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARY}
)

message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "OpenGL include dirs: ${OPENGL_INCLUDE_DIRS}")
message(STATUS "OpenGL libraries: ${OPENGL_LIBRARIES}")
message(STATUS "GLUT include dirs: ${GLUT_INCLUDE_DIRS}")
message(STATUS "GLUT library: ${GLUT_LIBRARY}")