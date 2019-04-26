cmake_minimum_required(VERSION 3.10)
project(glottal-inverse)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-pthread -fexceptions -Wall -Wextra -Wno-sign-compare -Werror=implicit-function-declaration -pipe -fPIC -fstack-protector-strong")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Os -Ofast -march=native")
endif()

include_directories(
    ${PROJECT_SOURCE_DIR}/inc
    ${PROJECT_SOURCE_DIR}/src
)

file(GLOB all_SRCS
    "${PROJECT_SOURCE_DIR}/inc/*.h"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/**/*.cpp"
)

add_executable(gif ${all_SRCS})
set_property(TARGET gif PROPERTY OUTPUT_NAME "${PROJECT_SOURCE_DIR}/bin/gif" )

set(CMAKE_EXE_LINKER_FLAGS "-pthread -Wl,-R .")
target_link_libraries(gif soundio asound pulse jack portaudio)
