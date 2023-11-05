cmake_minimum_required(VERSION 3.26)
project(GraphEngineGUI)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(ThirdParty)

add_executable(GraphEngineGUI main.cpp)
target_include_directories(GraphEngineGUI
        PRIVATE Include
        PRIVATE ${ThirdParty_Include_Dirs})
target_link_libraries(GraphEngineGUI
        PRIVATE ${ThirdParty_Libs})
