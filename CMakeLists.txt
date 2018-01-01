cmake_minimum_required(VERSION 3.0)
project (DolphinTrainer)

SET(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES 0)

set(SOURCE_DIRS					/structs
								/tools	)
						
set(BUILD_GUI TRUE CACHE BOOL "Build the GUI version?")

foreach(dir ${SOURCE_DIRS})
	aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}${dir} sources)
endforeach()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/cli CLI_SOURCES)

if (!WIN32)
	add_definitions(-DTRAINER_LINUX)
endif()

add_executable (DolphinTrainer ${sources} ${CLI_SOURCES})
target_link_libraries (DolphinTrainer psapi)
	
if(${BUILD_GUI})
	aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/gui GUI_SOURCES)
	add_executable(DolphinTrainerGUI ${sources} ${GUI_SOURCES})

	find_package(wxWidgets REQUIRED core base)
	include_directories(${wxWidgets_INCLUDE_DIRS})
	
	target_link_libraries(DolphinTrainerGUI ${wxWidgets_LIBRARIES})
	target_link_libraries(DolphinTrainerGUI psapi)	
endif()

list(APPEND CMAKE_CXX_FLAGS "--std=c++11")
#list(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,--subsystem,windows -mwindows")