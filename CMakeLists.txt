cmake_minimum_required(VERSION 3.16)

project(ddz-client VERSION 0.1 LANGUAGES CXX)

#自动处理.ui文件，最后生成ui_xxx.h文件
set(CMAKE_AUTOUIC ON)
#自动处理元对象相关的宏
set(CMAKE_AUTOMOC ON)
#自动处理qrc文件,将其生成为相应的c++代码，最后嵌入到可执行程序中
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#搜索QT模块
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
)

#添加头文件目录, 要写在add_subdirectory()上面
include_directories(
  ${PROJECT_SOURCE_DIR}/card
  ${PROJECT_SOURCE_DIR}/player
  ${PROJECT_SOURCE_DIR}/windows
  ${PROJECT_SOURCE_DIR}/control
  ${PROJECT_SOURCE_DIR}/thread
  ${PROJECT_SOURCE_DIR}/strategy
  ${PROJECT_SOURCE_DIR}/tcp
  ${PROJECT_SOURCE_DIR}/serialize
  ${PROJECT_SOURCE_DIR}/crypto
  ("E:\\OpenSSL-Win64\\include")
  ("C:\\protobuf\\include")
)


link_directories("C:\\protobuf\\lib")

#添加子目录
add_subdirectory(card)
add_subdirectory(player)
add_subdirectory(windows)
add_subdirectory(control)
add_subdirectory(thread)
add_subdirectory(strategy)
add_subdirectory(tcp)
add_subdirectory(serialize)
add_subdirectory(crypto)

#添加资源文件
qt_add_resources(PROJECT_SOURCES res.qrc)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ddz-client
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ddz-client APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ddz-client SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ddz-client
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ddz-client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets windows
  control player card strategy thread crypto serialize tcp)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ddz-client)
endif()
set_target_properties(ddz-client PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ddz-client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ddz-client)
endif()
