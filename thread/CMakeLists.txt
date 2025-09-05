cmake_minimum_required(VERSION 3.16)
project(thread)

aux_source_directory(./ SRC)
file(GLOB Headers ${PROJECT_SOURCE_DIR}/*.h)
add_library(thread ${SRC} ${Headers})
target_link_libraries(thread PRIVATE Qt${QT_VERSION_MAJOR}::Core)
