#pragma once

#include "imgui_impl_vulkan.h"

namespace GEGui::ApiSetup {

    /**
     *
     */
    class VulkanData {
    public:
        static VkAllocationCallbacks *g_Allocator;

        static VkInstance g_Instance;

        static VkPhysicalDevice g_PhysicalDevice;

        static VkDevice g_Device;

        static uint32_t g_QueueFamily;

        static VkQueue g_Queue;

        static VkDebugReportCallbackEXT g_DebugReport;

        static VkPipelineCache g_PipelineCache;

        static VkDescriptorPool g_DescriptorPool;

        static ImGui_ImplVulkanH_Window g_MainWindowData;

        static int g_MinImageCount;

        static bool g_SwapChainRebuild;
    };
}
