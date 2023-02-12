cmake_minimum_required(VERSION 3.7)

project(
    RWR
    LANGUAGES C
)

set(
    SRC

    "src/rwr.c"
    "src/sources.c"
    "src/tones.c"
    "src/model/alr56/alr56.c"
    "src/model/alr56/tones.c"
)

find_package(SDL2 REQUIRED)

add_library(rwr ${SRC})

target_link_libraries(rwr ${SDL2_LIBRARIES} m)
target_include_directories(rwr PUBLIC ${SDL2_INCLUDE_DIRECTORIES} "${CMAKE_SOURCE_DIR}/include")

add_executable(rwrsim "src/main.c")
target_link_libraries(rwrsim rwr)
