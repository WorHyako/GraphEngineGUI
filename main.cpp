#include <iostream>

#include "ApiSetup/VulkanSetuper.hpp"
#include "ApiSetup/VulkanData.hpp"
#include "ApiSetup/FrameBuffer.hpp"
#include "ApiSetup/ImGuiSetup.hpp"
#include "Controls/MainWindow.hpp"
#include "ApiSetup/FontLoader.hpp"

#include "WorLibrary/TemplateWrapper/Singleton.hpp"
#include "imgui_impl_glfw.h"
#include "GLFW/glfw3.h"

using namespace GEGui;

// Main code
int main(int, char **) {
    auto& mainWindow = Wor::TemplateWrapper::Singleton<Controls::MainWindow>::GetInstance();
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

    auto &imGuiSetup = Wor::TemplateWrapper::Singleton<ApiSetup::ImGuiSetup>::GetInstance();
    imGuiSetup.Init(mainWindow);

    ApiSetup::FontLoader::Load();

    // Our state
    bool showDemoWindow = true;
    bool showAnotherWindow = false;
    ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    VkResult err;
    // Main loop
    while (!glfwWindowShouldClose(mainWindow.GetWindow())) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        auto& wd = ApiSetup::VulkanData::g_MainWindowData;
        // Resize swap chain?
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

        // Start the Dear ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (showDemoWindow) {
            ImGui::ShowDemoWindow(&showDemoWindow);
        }
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            // Create a window called "Hello, world!" and append into it.
            ImGui::Begin("Hello, world!");
            // Display some text (you can use a format strings too)
            ImGui::Text("This is some useful text.");
            // Edit bools storing our window open/close state
            ImGui::Checkbox("Demo Window", &showDemoWindow);
            ImGui::Checkbox("Another Window", &showAnotherWindow);

            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            // Edit 3 floats representing a color
            ImGui::ColorEdit3("clear color", (float *) &clearColor);

            // Buttons return true when clicked (most widgets return true when edited/activated)
            if (ImGui::Button("Button")) {
                counter++;
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            auto &io = imGuiSetup.GetIO();
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (showAnotherWindow) {
            // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Begin("Another Window", &showAnotherWindow);
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me")) {
                showAnotherWindow = false;
            }
            ImGui::End();
        }

        // Rendering
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

    // Cleanup
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
