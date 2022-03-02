cmake_minimum_required(VERSION 3.17)
project(Life VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

add_executable(life "")

set_target_properties(
    life PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO

    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/"
)

add_subdirectory(extern)
add_subdirectory(src)
add_subdirectory(res)
