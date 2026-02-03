cmake_minimum_required(VERSION 3.16)

project(SM64AP-Launcher LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 11)

# Qt Setup
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

execute_process(COMMAND git log --pretty=format:%h -n 1 OUTPUT_VARIABLE GIT_REV)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/inc/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/inc/version.h)

qt_add_executable(SM64APLauncher
    src/main.cpp
    src/MainWindow.cpp
    src/BuildConfigurator.cpp
    src/BuildManager.cpp
    src/LogManager.cpp
    src/RequirementHandler.cpp
    src/ConfigManager.cpp
    src/PlatformRunner.cpp
    src/OutputWidget.cpp
)

# Linking
target_link_libraries(SM64APLauncher PRIVATE Qt6::Core Qt6::Widgets)
target_include_directories(SM64APLauncher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set_target_properties(SM64APLauncher PROPERTIES
    WIN32_EXECUTABLE ON
)

configure_file(res/metainfo.xml ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(res/SM64APLauncher.desktop ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

install(TARGETS SM64APLauncher)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/presets TYPE BIN USE_SOURCE_PERMISSIONS)
