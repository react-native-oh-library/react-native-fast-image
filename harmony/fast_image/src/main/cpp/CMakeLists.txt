# the minimum version of CMake
cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE on)

file(GLOB rnoh_fast_image_SRC CONFIGURE_DEPENDS *.cpp colorUtils/*.cpp)
add_library(rnoh_fast_image SHARED ${rnoh_fast_image_SRC})
target_include_directories(rnoh_fast_image PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rnoh_fast_image PUBLIC rnoh)