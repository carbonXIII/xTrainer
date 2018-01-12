cmake_minimum_required(VERSION 3.0)
project (DolphinTrainer)

SET(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES 0)

set(SOURCE_DIRS					/trainer
								/tools	)
						
foreach(dir ${SOURCE_DIRS})
	aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}${dir} sources)
endforeach()

if (!WIN32)
	add_definitions(-DTRAINER_LINUX)
endif()

add_executable (DolphinTrainer example.cpp ${sources})
target_link_libraries (DolphinTrainer psapi)

list(APPEND CMAKE_CXX_FLAGS "--std=c++11")
#list(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,--subsystem,windows -mwindows")