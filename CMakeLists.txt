cmake_minimum_required(VERSION 3.10)
project(glottal-inverse)

set(CMAKE_CXX_STANDARD 17)
add_definitions("-pthread -fexceptions -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -Werror=implicit-function-declaration -pipe -fPIC -fstack-protector-strong -DARMA_DONT_USE_WRAPPER")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions("-O2 -Ofast -g")
else()
    add_definitions("-O3 -Os -Ofast")
endif()

if(NOT TARGET_ARCH)
    set(TARGET_ARCH Linux)
endif()

include_directories(
    ${PROJECT_SOURCE_DIR}/inc
    ${PROJECT_SOURCE_DIR}/src
)

file(GLOB all_SRCS
    "${PROJECT_SOURCE_DIR}/inc/*.h"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/audio/**/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/filter/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/window/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/pitch/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/gci_pd/*.cpp"
)

add_executable(gif ${all_SRCS})
set_property(TARGET gif PROPERTY OUTPUT_NAME "${PROJECT_SOURCE_DIR}/bin/gif.${TARGET_ARCH}" )

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -pthread -Wl,-R .")

target_link_libraries(gif
    :libopenblas.a :liblapack.a
)

if(TARGET_ARCH MATCHES "^Android")
    add_definitions("-D__ANDROID__")

    target_link_libraries(gif
        :libgfortran.a
        gomp
        dl
        oboe
    )

    set (OBOE_DIR ./oboe)
    add_subdirectory(${OBOE_DIR} ./oboe)

    include_directories(${OBOE_DIR}/include)
else()
    target_link_libraries(gif
        stdc++fs
        gfortran
        soundio
        pulse
        portaudio
        asound
    )
endif()

