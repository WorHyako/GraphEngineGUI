#include "ApiSetup/ImGuiSetup.hpp"

#include "ApiSetup/VulkanData.hpp"
#include "ApiSetup/VulkanSetuper.hpp"

#include "imgui_impl_glfw.h"

using namespace GEGui::ApiSetup;

void ImGuiSetup::Create() noexcept {
    IMGUI_CHECKVERSION();
    context = std::make_unique<ImGuiContext *>(ImGui::CreateContext());
    _io = &ImGui::GetIO();
    (void) _io;
    // Enable Keyboard Controls
    _io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Enable Gamepad Controls
    _io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
}

void ImGuiSetup::SetupColor() noexcept {
    ImGui::StyleColorsDark();
}

void ImGuiSetup::Init(const Controls::MainWindow& window) noexcept {
    Create();
    ImGui_ImplGlfw_InitForVulkan(window.GetWindow(), true);
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
            .ImageCount = VulkanData::g_MainWindowData.ImageCount,
            .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
            .Allocator = ApiSetup::VulkanData::g_Allocator,
            .CheckVkResultFn = ApiSetup::VulkanSetuper::CheckVkResult
    };
    ImGui_ImplVulkan_Init(&initInfo, VulkanData::g_MainWindowData.RenderPass);
}

#pragma region Accessors

ImGuiIO &ImGuiSetup::GetIO() noexcept {
    return *_io;
}

#pragma endregion Accessors
