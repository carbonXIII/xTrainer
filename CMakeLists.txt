cmake_minimum_required(VERSION 3.0)
project (xTrainer)

set(SOURCE_DIRS					/trainer
								/tools )
						
foreach(dir ${SOURCE_DIRS})
	aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}${dir} sources)
endforeach()
include_directories(SOURCE_DIRS)

if (!WIN32)
	add_definitions(-DTRAINER_LINUX)
endif()

add_library (xTrainer ${sources})

install(TARGETS xTrainer DESTINATION lib)
foreach(dir ${SOURCE_DIRS})
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}${dir} DESTINATION include/xTrainer FILES_MATCHING PATTERN "*.h")
endforeach()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/xTrainer.h DESTINATION include/xTrainer)

set(BUILD_SAMPLES TRUE CACHE BOOL "Build sample programs")

if(${BUILD_SAMPLES})
	add_subdirectory(samples samples)
endif()


list(APPEND CMAKE_CXX_FLAGS "--std=c++11 -g")
#list(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,--subsystem,windows -mwindows")