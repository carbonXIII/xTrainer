cmake_minimum_required(VERSION 3.1)
project (xtrainer)

set (CMAKE_CXX_STANDARD 11)

set(SOURCE_DIRS	/trainer /tools)

aux_source_directory(
  ${CMAKE_CURRENT_SOURCE_DIR}/tools XTRAINER_SOURCES)
aux_source_directory(
  ${CMAKE_CURRENT_SOURCE_DIR}/trainer XTRAINER_SOURCES)

add_library (xtrainer ${XTRAINER_SOURCES})

if(NOT WIN32)
	target_compile_definitions(xtrainer PUBLIC -DTRAINER_LINUX)
endif()

target_include_directories(
  xtrainer PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

target_include_directories(
  xtrainer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/xtrainer)

install(TARGETS xtrainer DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION include/)

if(WIN32)
	target_link_libraries (xtrainer PUBLIC psapi)
endif()

set(BUILD_SAMPLES TRUE CACHE BOOL "Build sample programs")

if(${BUILD_SAMPLES})
	add_subdirectory(samples samples)
endif()
