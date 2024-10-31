# the minimum version of CMake
cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE on)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()
file(GLOB rnoh_fast_image_SRC CONFIGURE_DEPENDS *.cpp colorUtils/*.cpp downloadUtils/*.cpp)
add_library(rnoh_fast_image SHARED ${rnoh_fast_image_SRC})
target_include_directories(rnoh_fast_image PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rnoh_fast_image PUBLIC rnoh)

target_link_libraries(rnoh_fast_image PUBLIC librcp_c.so
                                            libohfileuri.so libimage_source.so
                                             libace_ndk.z.so )

target_link_directories(rnoh_fast_image PUBLIC ${HMOS_SDK_NATIVE}/sysroot/usr/lib/aarch64-linux-ohos)