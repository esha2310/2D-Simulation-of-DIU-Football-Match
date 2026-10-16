cmake_minimum_required(VERSION 3.16)
project(DIUFootballMatch2D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(diu_football_match src/main.cpp)

target_link_libraries(
    diu_football_match
    PRIVATE
    OpenGL::GL
    GLUT::GLUT
)
