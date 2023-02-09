cmake_minimum_required(VERSION 3.7)

project(
    RWR
    LANGUAGES C
)

set(
    SRC

    "src/rwr.c"
)

find_package(SDL2 REQUIRED)

add_library(rwr ${SRC})

target_link_libraries(rwr ${SDL2_LIBRARIES})
target_include_directories(rwr PUBLIC ${SDL2_INCLUDE_DIRECTORIES})

add_executable(rwrsim "src/main.c")
target_link_libraries(rwrsim rwr)
