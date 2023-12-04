#include "ApiSetup/FontLoader.hpp"

#include "vulkan/vulkan.h"

#include "ApiSetup/VulkanSetuper.hpp"
#include "ApiSetup/VulkanData.hpp"

using namespace GEGui::ApiSetup;

void FontLoader::Load() noexcept {
    VkResult err;
    auto &windowData = ApiSetup::VulkanData::g_MainWindowData;
    VkCommandPool commandPool = windowData.Frames[windowData.FrameIndex].CommandPool;
    VkCommandBuffer commandBuffer = windowData.Frames[windowData.FrameIndex].CommandBuffer;

    err = vkResetCommandPool(ApiSetup::VulkanData::g_Device, commandPool, 0);
    ApiSetup::VulkanSetuper::CheckVkResult(err);
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    err = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    ApiSetup::VulkanSetuper::CheckVkResult(err);

    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

    VkSubmitInfo endInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer
    };
    err = vkEndCommandBuffer(commandBuffer);
    ApiSetup::VulkanSetuper::CheckVkResult(err);
    err = vkQueueSubmit(ApiSetup::VulkanData::g_Queue, 1, &endInfo, VK_NULL_HANDLE);
    ApiSetup::VulkanSetuper::CheckVkResult(err);

    err = vkDeviceWaitIdle(ApiSetup::VulkanData::g_Device);
    ApiSetup::VulkanSetuper::CheckVkResult(err);
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}
