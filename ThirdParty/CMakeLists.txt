##############
### Vulkan ###
##############
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")
find_package(Vulkan REQUIRED)
list(APPEND ThirdParty_Libs ${Vulkan_LIBRARIES})
list(APPEND ThirdParty_Include_Dirs ${Vulkan_INCLUDE_DIRS})

############
### glfw ###
############
set(Glfw_Root ${CMAKE_CURRENT_SOURCE_DIR}/glfw)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
set(GLFW_DOCUMENT_INTERNALS OFF)
add_subdirectory(${Glfw_Root})
list(APPEND ThirdParty_Libs glfw)

#############
### ImGui ###
#############
set(ImGui_Root ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
file(GLOB Imgui_Sources ${ImGui_Root}/*.*pp)
if (APPLE)
    list(APPEND Imgui_Sources
            ${ImGui_Root}/backends/imgui_impl_vulkan.cpp
            ${ImGui_Root}/backends/imgui_impl_glfw.cpp
            ${ImGui_Root}/backends/imgui_impl_opengl3.cpp)
endif ()

add_library(imgui STATIC)
target_sources(imgui
        PRIVATE ${Imgui_Sources})
target_include_directories(imgui
        PUBLIC ${Vulkan_INCLUDE_DIRS}
        PUBLIC ${ImGui_Root}
        PUBLIC ${ImGui_Root}/include
        PUBLIC ${ImGui_Root}/backends)
target_link_libraries(imgui
        PUBLIC ${Vulkan_LIBRARIES}
        PUBLIC glfw)

list(APPEND ThirdParty_Libs imgui)

set(ThirdParty_Libs ${ThirdParty_Libs} PARENT_SCOPE)
set(ThirdParty_Include_Dirs ${ThirdParty_Include_Dirs} PARENT_SCOPE)
