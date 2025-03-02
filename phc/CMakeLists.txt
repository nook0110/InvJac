cmake_minimum_required(VERSION 3.12)

project(phcpack_c_interface CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FILE(GLOB SOURCE_LIB phc/src/C/*.c)

add_compile_definitions(compilewgpp=1)
set_source_files_properties(${SOURCE_LIB} PROPERTIES LANGUAGE CXX)

add_library(phcpack_c_interface STATIC ${SOURCE_LIB})
target_link_libraries(phcpack_c_interface libPHCpack.so)
