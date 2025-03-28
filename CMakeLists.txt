cmake_minimum_required(VERSION 3.12)

project(invjac CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(compilewgpp=1)

FILE(GLOB SOURCES source/*.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.45.0 COMPONENTS program_options) 

include_directories(${Boost_INCLUDE_DIRS}) 
add_library(invjac_lib ${SOURCES})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/phc/PHCpack/lib)
set(LIBRARIES phcpack_c_interface libPHCpack.so cln ginac mysql::concpp glog::glog ${Boost_LIBRARIES})
target_link_libraries(invjac_lib ${LIBRARIES})

add_executable(invjac source/main.cpp)    


include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(phc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/phc/PHCpack/src/C)


set(mysql-concpp_DIR "/usr/local/mysql-connector")
find_package(mysql-concpp REQUIRED)
include_directories(mysql-concpp)

find_package(glog 0.7.1 REQUIRED)
target_link_libraries(invjac invjac_lib)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

FILE(GLOB TEST_SOURCES tests/*.cpp)
enable_testing()
add_executable(invjac_test ${TEST_SOURCES})
target_link_libraries(invjac_test invjac_lib GTest::gtest_main)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(invjac_test PRIVATE --coverage -g)
    target_link_options(invjac_test PRIVATE --coverage)
    target_compile_options(invjac_lib PRIVATE --coverage -g)
    target_link_options(invjac_lib PRIVATE --coverage)
  endif()
endif()

add_custom_target(coverage
    COMMAND lcov --capture --directory . --output-file coverage.info --ignore-errors mismatch
    COMMAND lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
    COMMAND lcov --extract coverage.info '${CMAKE_SOURCE_DIR}/source/*' --output-file coverage.info
    COMMAND genhtml coverage.info --output-directory ${CMAKE_SOURCE_DIR}/docs/coverage_report
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating code coverage report"
)

include(GoogleTest)
gtest_discover_tests(invjac_test)