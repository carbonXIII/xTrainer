cmake_minimum_required(VERSION 3.1)
project (xTrainer)

set(SOURCE_DIRS					/trainer
                        /tools
                        /platform/linux
                        /core)

set(INCLUDE_DIRS		  	/trainer
                        /tools
                        /platform
                        /core)

foreach(dir ${SOURCE_DIRS})
	aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}${dir} sources)
endforeach()

include_directories(INCLUDE_DIRS)

set (CMAKE_CXX_STANDARD 11)
add_library (xTrainer ${sources})

if(NOT WIN32)
	add_definitions(-DTRAINER_LINUX)
endif()

install(TARGETS xTrainer DESTINATION lib)
foreach(dir ${SOURCE_DIRS})
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}${dir} DESTINATION include/xTrainer FILES_MATCHING PATTERN "*.h")
endforeach()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/xTrainer.h DESTINATION include/xTrainer)

set(BUILD_SAMPLES TRUE CACHE BOOL "Build sample programs")

if(${BUILD_SAMPLES})
	add_subdirectory(samples samples)
endif()
