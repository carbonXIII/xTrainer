cmake_minimum_required(VERSION 3.0)
project (DolphinTrainer)

add_executable (DolphinTrainer main.cpp)
target_link_libraries (DolphinTrainer psapi)

list( APPEND CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")