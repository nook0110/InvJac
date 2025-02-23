cmake_minimum_required(VERSION 3.12)

project(invjac CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -O3")

add_compile_definitions(compilewgpp=1)

FILE(GLOB SOURCES source/*.cpp)
list(REMOVE_ITEM SOURCES source/main.cpp)

add_library(invjac_lib ${SOURCES})
set(LIBRARIES phcpack_c_interface libPHCpack.so cln ginac mysql::concpp glog::glog)
target_link_libraries(invjac_lib ${LIBRARIES})

add_executable(invjac source/main.cpp)    


include_directories(.)
add_subdirectory(phc)

set(mysql-concpp_DIR "mysql-connector")
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

include(GoogleTest)
gtest_discover_tests(invjac_test)