#include "ApiSetup/VulkanData.hpp"

using namespace GEGui::ApiSetup;

VkAllocationCallbacks *VulkanData::g_Allocator = nullptr;

VkInstance VulkanData::g_Instance = VK_NULL_HANDLE;

VkPhysicalDevice VulkanData::g_PhysicalDevice = VK_NULL_HANDLE;

VkDevice VulkanData::g_Device = VK_NULL_HANDLE;

uint32_t VulkanData::g_QueueFamily = static_cast<uint32_t>(-1);

VkQueue VulkanData::g_Queue = VK_NULL_HANDLE;

VkDebugReportCallbackEXT VulkanData::g_DebugReport = VK_NULL_HANDLE;

VkPipelineCache VulkanData::g_PipelineCache = VK_NULL_HANDLE;

VkDescriptorPool VulkanData::g_DescriptorPool = VK_NULL_HANDLE;

ImGui_ImplVulkanH_Window VulkanData::g_MainWindowData;

int VulkanData::g_MinImageCount = 2;

bool VulkanData::g_SwapChainRebuild = false;
