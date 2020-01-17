cmake_minimum_required(VERSION 3.11)
project(xtrainer)

add_library(xtrainer INTERFACE)

target_include_directories(xtrainer INTERFACE ${CMAKE_SOURCE_DIR})

add_subdirectory(test)
