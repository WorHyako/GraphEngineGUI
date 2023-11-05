#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "GLFW/glfw3.h"

#include <vulkan/vulkan.h>

#include <iostream>

#include "ApiSetup/VulkanSetuper.hpp"
#include "ApiSetup/VulkanData.hpp"

using namespace GEGui;

// Main code
int main(int, char **) {
    glfwSetErrorCallback(ApiSetup::VulkanSetuper::GlfwErrorCallback);
    if (!glfwInit())
        return 1;

    // Create window with Vulkan context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+Vulkan example", nullptr, nullptr);
    if (!glfwVulkanSupported()) {
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }

    ImVector<const char *> extensions;
    uint32_t extensionsCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionsCount);
    for (uint32_t i = 0; i < extensionsCount; i++)
        extensions.push_back(glfwExtensions[i]);
    GEGui::ApiSetup::VulkanSetuper::SetupVulkan(extensions);

    // Create Window Surface
    VkSurfaceKHR surface;
    VkResult err = glfwCreateWindowSurface(ApiSetup::VulkanData::g_Instance,
                                           window,
                                           ApiSetup::VulkanData::g_Allocator,
                                           &surface);
    ApiSetup::VulkanSetuper::CheckVkResult(err);

    // Create Framebuffers
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    ImGui_ImplVulkanH_Window *wd = &ApiSetup::VulkanData::g_MainWindowData;
    ApiSetup::VulkanSetuper::SetupVulkanWindow(wd, surface, w, h);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo initInfo {
            .Instance = ApiSetup::VulkanData::g_Instance,
            .PhysicalDevice = ApiSetup::VulkanData::g_PhysicalDevice,
            .Device = ApiSetup::VulkanData::g_Device,
            .QueueFamily = ApiSetup::VulkanData::g_QueueFamily,
            .Queue = ApiSetup::VulkanData::g_Queue,
            .PipelineCache = ApiSetup::VulkanData::g_PipelineCache,
            .DescriptorPool = ApiSetup::VulkanData::g_DescriptorPool,
            .Subpass = 0,
            .MinImageCount = static_cast<std::uint32_t>(ApiSetup::VulkanData::g_MinImageCount),
            .ImageCount = wd->ImageCount,
            .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
            .Allocator = ApiSetup::VulkanData::g_Allocator,
            .CheckVkResultFn = ApiSetup::VulkanSetuper::CheckVkResult
    };
    ImGui_ImplVulkan_Init(&initInfo, wd->RenderPass);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Upload Fonts
    {
        // Use any command queue
        VkCommandPool commandPool = wd->Frames[wd->FrameIndex].CommandPool;
        VkCommandBuffer commandBuffer = wd->Frames[wd->FrameIndex].CommandBuffer;

        err = vkResetCommandPool(ApiSetup::VulkanData::g_Device, commandPool, 0);
        ApiSetup::VulkanSetuper::CheckVkResult(err);
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer(commandBuffer, &beginInfo);
        ApiSetup::VulkanSetuper::CheckVkResult(err);

        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

        VkSubmitInfo end_info = {};
        end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        end_info.commandBufferCount = 1;
        end_info.pCommandBuffers = &commandBuffer;
        err = vkEndCommandBuffer(commandBuffer);
        ApiSetup::VulkanSetuper::CheckVkResult(err);
        err = vkQueueSubmit(ApiSetup::VulkanData::g_Queue, 1, &end_info, VK_NULL_HANDLE);
        ApiSetup::VulkanSetuper::CheckVkResult(err);

        err = vkDeviceWaitIdle(ApiSetup::VulkanData::g_Device);
        ApiSetup::VulkanSetuper::CheckVkResult(err);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    // Our state
    bool showDemoWindow = true;
    bool showAnotherWindow = false;
    ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Resize swap chain?
        if (ApiSetup::VulkanData::g_SwapChainRebuild) {
            int width;
            int height;
            glfwGetFramebufferSize(window, &width, &height);
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
            wd->ClearValue.color.float32[0] = clearColor.x * clearColor.w;
            wd->ClearValue.color.float32[1] = clearColor.y * clearColor.w;
            wd->ClearValue.color.float32[2] = clearColor.z * clearColor.w;
            wd->ClearValue.color.float32[3] = clearColor.w;
            ApiSetup::VulkanSetuper::FrameRender(wd, drawData);
            ApiSetup::VulkanSetuper::FramePresent(wd);
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

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
