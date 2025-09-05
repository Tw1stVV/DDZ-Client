cmake_minimum_required(VERSION 3.16)
project(card)
aux_source_directory(./ SRC)
file(GLOB Headers ${PROJECT_SOURCE_DIR}/*.h)
add_library(card ${SRC} ${Headers})

target_link_libraries(card PRIVATE Qt${QT_VERSION_MAJOR}::Core)
