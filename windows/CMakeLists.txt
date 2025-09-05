cmake_minimum_required(VERSION 3.16)
project(windows)
aux_source_directory(./ SRC)
file(GLOB Headers ${PROJECT_SOURCE_DIR}/*.h)
file(GLOB Uis ${PROJECT_SOURCE_DIR}/*.ui)
add_library(windows ${SRC} ${Headers} ${Uis})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)
target_link_libraries(windows PRIVATE Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
)
