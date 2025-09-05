cmake_minimum_required(VERSION 3.16)
project(control)
aux_source_directory(./ SRC)
file(GLOB Headers ${PROJECT_SOURCE_DIR}/*.h)
add_library(control ${SRC} ${Headers})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)
target_link_libraries(control PRIVATE Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Multimedia
)
