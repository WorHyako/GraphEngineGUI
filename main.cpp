#include <iostream>

#include "ApiSetup/VulkanSetuper.hpp"
#include "ApiSetup/VulkanData.hpp"
#include "ApiSetup/FrameBuffer.hpp"
#include "ApiSetup/ImGuiSetup.hpp"
#include "Controls/MainWindow.hpp"
#include "ApiSetup/FontLoader.hpp"
#include "Widgets/ContentBrowserPanel.hpp"
#include "Style/Icons/IconIDs.hpp"

#include "WorLibrary/TemplateWrapper/Singleton.hpp"

#include "imgui_impl_glfw.h"
#include "GLFW/glfw3.h"

using namespace GEGui;
using namespace Wor::TemplateWrapper;

// Main code
int main(int, char **) {
    auto &mainWindow = Singleton<Controls::MainWindow>::getInstance();
    if (!mainWindow.Init()) {
        return -300;
    }

    auto setupVulkanRes = ApiSetup::VulkanSetuper::SetupVulkan();
//    if (setupVulkanRes != ApiSetup::VulkanSetuper::Status::Setuped) {
//        return -200;
//    }

    mainWindow.CreateWindowSurface();

    const auto size = mainWindow.GetFrameSize();
    ApiSetup::VulkanSetuper::SetupVulkanWindow(&ApiSetup::VulkanData::g_MainWindowData,
                                               mainWindow.GetSurface(),
                                               size.x,
                                               size.y);

    auto &imGuiSetup = Singleton<ApiSetup::ImGuiSetup>::getInstance();
    imGuiSetup.Init(mainWindow);


    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->AddFontDefault();

    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = 13.0f;
    static const ImWchar iconRanges[] = { Style::Icons::IconMin, Style::Icons::IconMax };
    io.Fonts->AddFontFromFileTTF("OpenFontIcons.ttf", 50.0f, &config, iconRanges);
    io.Fonts->Build();

    ApiSetup::FontLoader::Load();

    ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    Widgets::ContentBrowserPanel contentBrowserPanel;

    VkResult err;
    while (!glfwWindowShouldClose(mainWindow.GetWindow())) {
        glfwPollEvents();

        auto &wd = ApiSetup::VulkanData::g_MainWindowData;
        if (ApiSetup::VulkanData::g_SwapChainRebuild) {
            int width;
            int height;
            glfwGetFramebufferSize(mainWindow.GetWindow(), &width, &height);
            if (width > 0 && height > 0) {
                ImGui_ImplVulkan_SetMinImageCount(ApiSetup::VulkanData::g_MinImageCount);
                ImGui_ImplVulkanH_CreateOrResizeWindow(ApiSetup::VulkanData::g_Instance,
                                                       ApiSetup::VulkanData::g_PhysicalDevice,
                                                       ApiSetup::VulkanData::g_Device,
                                                       &ApiSetup::VulkanData::g_MainWindowData,
                                                       ApiSetup::VulkanData::g_QueueFamily,
                                                       ApiSetup::VulkanData::g_Allocator,
                                                       width,
                                                       height,
                                                       ApiSetup::VulkanData::g_MinImageCount);
                ApiSetup::VulkanData::g_MainWindowData.FrameIndex = 0;
                ApiSetup::VulkanData::g_SwapChainRebuild = false;
            }
        }

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        contentBrowserPanel.Render();

        ImGui::Render();
        ImDrawData *drawData = ImGui::GetDrawData();
        const bool isMinimized = (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f);
        if (!isMinimized) {
            wd.ClearValue.color.float32[0] = clearColor.x * clearColor.w;
            wd.ClearValue.color.float32[1] = clearColor.y * clearColor.w;
            wd.ClearValue.color.float32[2] = clearColor.z * clearColor.w;
            wd.ClearValue.color.float32[3] = clearColor.w;
            ApiSetup::VulkanSetuper::FrameRender(&wd, drawData);
            ApiSetup::VulkanSetuper::FramePresent(&wd);
        }
    }

    err = vkDeviceWaitIdle(ApiSetup::VulkanData::g_Device);
    ApiSetup::VulkanSetuper::CheckVkResult(err);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    GEGui::ApiSetup::VulkanSetuper::CleanupVulkanWindow();
    GEGui::ApiSetup::VulkanSetuper::CleanupVulkan();

    glfwDestroyWindow(mainWindow.GetWindow());

    return 0;
}
