cmake_minimum_required(VERSION 3.0)
project (notepadtrainer)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} sources)
add_executable(notepadtrainer ${sources})

target_link_libraries(notepadtrainer xtrainer)
