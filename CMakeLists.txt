cmake_minimum_required(VERSION 3.27)
project(diffuse)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(SOURCE_FILES
    src/point.cpp
    src/curves/curve.cpp
    src/curves/bezier.cpp
    src/curves/bspline.cpp
    src/curves/catmullrom.cpp
    src/curves/lagrange.cpp
    src/diffuse.cpp
    src/colorpicker.cpp
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::OpenGL OpenGL::GLU GLUT::GLUT)
