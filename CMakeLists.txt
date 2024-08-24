cmake_minimum_required(VERSION 3.10)

project(minidb VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

add_executable(minidb ${SOURCES} ${HEADERS})

target_include_directories(minidb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(minidb PRIVATE -Wall -Wextra)