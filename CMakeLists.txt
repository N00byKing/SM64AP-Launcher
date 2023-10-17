cmake_minimum_required(VERSION 3.16)

project(SM64AP-Launcher LANGUAGES CXX VERSION 0.0.1)

# Qt Setup
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

qt_add_executable(SM64APLauncher
    src/main.cpp
    src/MainWindow.cpp
    inc/MainWindow.h
    src/BuildConfigurator.cpp
    inc/BuildConfigurator.h
    src/LogWindow.cpp
    inc/LogWindow.h
    src/RequirementHandler.cpp
    inc/RequirementHandler.h
)

target_link_libraries(SM64APLauncher PRIVATE Qt6::Core Qt6::Widgets)
target_include_directories(SM64APLauncher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set_target_properties(SM64APLauncher PROPERTIES
    WIN32_EXECUTABLE ON
)
