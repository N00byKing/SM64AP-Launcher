cmake_minimum_required(VERSION 3.16)

project(SM64AP-Launcher LANGUAGES CXX VERSION 0.0.1)

# Qt Setup
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

execute_process(COMMAND git log --pretty=format:%h -n 1 OUTPUT_VARIABLE GIT_REV)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/inc/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/inc/version.h)

qt_add_executable(SM64APLauncher
    src/main.cpp
    src/MainWindow.cpp
    src/BuildConfigurator.cpp
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

install(FILES $<TARGET_FILE:SM64APLauncher> DESTINATION SM64APLauncher_install)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/presets DESTINATION SM64APLauncher_install)

if(CMAKE_HOST_WIN32)
    add_custom_command(
        TARGET SM64APLauncher POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/launcher_deploy
        COMMAND windeployqt.exe --release --compiler-runtime --dir ${CMAKE_CURRENT_BINARY_DIR}/launcher_deploy $<TARGET_FILE:SM64APLauncher>
    )
    install(
        DIRECTORY ${CMAKE_BINARY_DIR}/launcher_deploy/
        DESTINATION SM64APLauncher_install
    )
endif()
