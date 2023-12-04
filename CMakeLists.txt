cmake_minimum_required(VERSION 3.25)
project(GraphEngineGUI)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(ThirdParty)

include(CMakeUtils/FolderConfigurator.cmake)
ConfigureProjectFolders()
CopyResourcesToEngine()

file(GLOB_RECURSE GraphEngineGUI_Sources Src/*.cpp)

add_executable(GraphEngineGUI main.cpp)
target_sources(GraphEngineGUI
        PRIVATE
        ${GraphEngineGUI_Sources})
target_include_directories(GraphEngineGUI
        PRIVATE
        Include
        ${ThirdParty_Include_Dirs})
target_link_libraries(GraphEngineGUI
        PRIVATE
        ${ThirdParty_Libs})
