cmake_minimum_required(VERSION 3.10)
project(bunnycoin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_define_targets()

find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Network CONFIG REQUIRED)

add_subdirectory(src)
