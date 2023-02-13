cmake_minimum_required(VERSION 3.7)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

project(
    RWR
    LANGUAGES C
)

# Creates C resources file from files in given directory
function(create_resources dir output)
    # Create empty output file
    file(WRITE ${output} "")
    # Collect input files
    file(GLOB bins ${dir}/*)
    # Iterate through input files
    foreach(bin ${bins})
        # Get short filename
        string(REGEX MATCH "([^/]+)$" filename ${bin})
        # Replace filename spaces & extension separator for C compatibility
        string(REGEX REPLACE "\\.| |-" "_" filename ${filename})
        # Read hex data from file
        file(READ ${bin} filedata HEX)
        # Convert hex data for C compatibility
        string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
        # Append data to output file
        file(APPEND ${output} "const unsigned char ${filename}[] = {${filedata}};\nconst unsigned ${filename}_size = sizeof(${filename});\n")
    endforeach()
endfunction()


set(
    SRC

    "src/rwr.c"
    "src/sources.c"
    "src/tones.c"
    "src/model/alr56/alr56.c"
    "src/model/alr56/tones.c"
    "src/model/alr56/render.c"
)

create_resources("${CMAKE_SOURCE_DIR}/asset/icon" "${CMAKE_BINARY_DIR}/generated/icons.h")
create_resources("${CMAKE_SOURCE_DIR}/asset/panel" "${CMAKE_BINARY_DIR}/generated/panels.h")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_library(rwr ${SRC})

target_link_libraries(rwr ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} m)
target_include_directories(rwr PUBLIC ${SDL2_INCLUDE_DIRECTORIES} ${SDL2_IMAGE_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include" "${CMAKE_BINARY_DIR}/generated")

add_executable(rwrsim "src/main.c")
target_link_libraries(rwrsim rwr)
