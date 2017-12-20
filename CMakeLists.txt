cmake_minimum_required(VERSION 3.0)
project (DolphinTrainer)

set(SOURCE_DIRS					/.
								/cli
								/structs
								/tools	)

foreach (dir ${SOURCE_DIRS})
	aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}${dir} sources)
endforeach (dir ${SOURCE_DIRS})
message("Files: ${sources}")

if (!WIN32)
	add_definitions(-DTRAINER_LINUX)
endif (!WIN32)

add_executable (DolphinTrainer ${sources})
target_link_libraries (DolphinTrainer psapi)

list( APPEND CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")