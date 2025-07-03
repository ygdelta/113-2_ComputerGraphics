cmake_minimum_required(VERSION 2.8)
project(main)

# 設定 C++ 標準
set(CMAKE_CXX_STANDARD 11)

# 尋找 OpenGL
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)

# 對於 FreeGLUT，我們需要特別處理
# 方法 1: 先嘗試使用 find_package
find_package(GLUT)

# 方法 2: 如果失敗，手動尋找 FreeGLUT
if(NOT GLUT_FOUND)
    message(STATUS "GLUT not found by find_package, searching for FreeGLUT manually...")
    
    find_path(FREEGLUT_INCLUDE_DIR 
        NAMES GL/freeglut.h GL/glut.h
        PATHS 
        /usr/include
        /usr/local/include
        /usr/include/GL
        /usr/local/include/GL
    )
    
    find_library(FREEGLUT_LIBRARY
        NAMES freeglut glut
        PATHS
        /usr/lib
        /usr/local/lib
        /usr/lib/x86_64-linux-gnu
        /usr/lib/i386-linux-gnu
        /usr/lib/arm-linux-gnueabihf
        /usr/lib/aarch64-linux-gnu
    )
    
    if(FREEGLUT_INCLUDE_DIR AND FREEGLUT_LIBRARY)
        set(GLUT_FOUND TRUE)
        set(GLUT_INCLUDE_DIR ${FREEGLUT_INCLUDE_DIR})
        set(GLUT_LIBRARIES ${FREEGLUT_LIBRARY})
        message(STATUS "Found FreeGLUT manually")
        message(STATUS "  Include: ${FREEGLUT_INCLUDE_DIR}")
        message(STATUS "  Library: ${FREEGLUT_LIBRARY}")
    endif()
endif()

# 檢查是否找到所有必要的函式庫
if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "OpenGL not found!")
endif()

if(NOT GLUT_FOUND)
    message(FATAL_ERROR "GLUT/FreeGLUT not found! Please install freeglut3-dev")
endif()

# 輸出找到的函式庫資訊
message(STATUS "OpenGL found:")
message(STATUS "  Include: ${OPENGL_INCLUDE_DIR}")
message(STATUS "  Libraries: ${OPENGL_LIBRARIES}")
message(STATUS "GLUT found:")
message(STATUS "  Include: ${GLUT_INCLUDE_DIR}")
message(STATUS "  Libraries: ${GLUT_LIBRARIES}")

# 設定 include 目錄
include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${GLUT_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR} 
)

# 加入執行檔
add_executable(main sphereworld.cpp)
add_library(math3d math3d.cpp)

# 連結函式庫 - 順序很重要！
target_link_libraries(main 
    ${GLUT_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${CMAKE_DL_LIBS}
    ${OpenCV_LIBS}
    m
    math3d
)

message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "OpenGL include dirs: ${OPENGL_INCLUDE_DIRS}")
message(STATUS "OpenGL libraries: ${OPENGL_LIBRARIES}")
message(STATUS "GLUT include dirs: ${GLUT_INCLUDE_DIRS}")
message(STATUS "GLUT library: ${GLUT_LIBRARY}")