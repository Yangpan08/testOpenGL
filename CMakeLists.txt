cmake_minimum_required (VERSION 3.8)
project ("OpenGLProject")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/target)

add_subdirectory("source")
