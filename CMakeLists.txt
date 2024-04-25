cmake_minimum_required(VERSION 3.22)
project(Count2)

set(CMAKE_CXX_STANDARD 17)

set(SFML_DIR "C:/Users/Ruslan/CLionProjects/lib/sfml/GCC 7.3.0 MinGW (SEH) - 64-bit/SFML-2.5.1/lib/cmake/SFML")

add_executable(Count2 main.cpp)

set(SFML_DIR "C:/Users/Ruslan/CLionProjects/lib/sfml/GCC 7.3.0 MinGW (SEH) - 64-bit/SFML-2.5.1/lib/cmake/SFML")

find_package(GLEW REQUIRED)
target_link_libraries(Count2 ${GLEW_LIBRARIES})

find_package(OpenGL REQUIRED)
target_link_libraries(Count2 ${OPENGL_LIBRARY})

target_link_libraries (Count2 sfml-graphics sfml-audio)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

find_package(SFML 2.5.1 REQUIRED graphics network audio window system)