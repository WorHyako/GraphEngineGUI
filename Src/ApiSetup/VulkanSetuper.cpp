#include "ApiSetup/VulkanSetuper.hpp"

#include "ApiSetup/VulkanData.hpp"

#include "GLFW/glfw3.h"

#include <vector>

using namespace GEGui::ApiSetup;

VulkanSetuper::Status VulkanSetuper::SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd,
                                      VkSurfaceKHR surface,
                                      int width,
                                      int height) noexcept {
    wd->Surface = surface;

    VkBool32 res;
    vkGetPhysicalDeviceSurfaceSupportKHR(VulkanData::g_PhysicalDevice,
                                         VulkanData::g_QueueFamily,
                                         wd->Surface,
                                         &res);
    if (res != VK_TRUE) {
        fprintf(stderr, "Error no WSI support on physical device 0\n");
        exit(-1);
    }

    const std::vector<VkFormat> requestSurfaceImageFormat { VK_FORMAT_B8G8R8A8_UNORM,
                                                            VK_FORMAT_R8G8B8A8_UNORM,
                                                            VK_FORMAT_B8G8R8_UNORM,
                                                            VK_FORMAT_R8G8B8_UNORM };
    constexpr VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(VulkanData::g_PhysicalDevice, wd->Surface,
                                                              requestSurfaceImageFormat.data(),
                                                              (size_t) IM_ARRAYSIZE(requestSurfaceImageFormat.begin()),
                                                              requestSurfaceColorSpace);

#ifdef IMGUI_UNLIMITED_FRAME_RATE
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
    wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(VulkanData::g_PhysicalDevice,
                                                          wd->Surface,
                                                          &present_modes[0],
                                                          IM_ARRAYSIZE(present_modes));

    IM_ASSERT(VulkanData::g_MinImageCount >= 2);
    ImGui_ImplVulkanH_CreateOrResizeWindow(VulkanData::g_Instance,
                                           VulkanData::g_PhysicalDevice,
                                           VulkanData::g_Device,
                                           wd,
                                           VulkanData::g_QueueFamily,
                                           VulkanData::g_Allocator,
                                           width,
                                           height,
                                           VulkanData::g_MinImageCount);
}

void VulkanSetuper::CleanupVulkan() noexcept {
    vkDestroyDescriptorPool(VulkanData::g_Device,
                            VulkanData::g_DescriptorPool,
                            VulkanData::g_Allocator);

#ifdef IMGUI_VULKAN_DEBUG_REPORT
    // Remove the debug report callback
    auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(g_Instance, "vkDestroyDebugReportCallbackEXT");
    vkDestroyDebugReportCallbackEXT(VulkanData::g_Instance,
                                    VulkanData::g_DebugReport,
                                    VulkanData::g_Allocator);
#endif

    vkDestroyDevice(VulkanData::g_Device,
                    VulkanData::g_Allocator);
    vkDestroyInstance(VulkanData::g_Instance,
                      VulkanData::g_Allocator);
}

void VulkanSetuper::CleanupVulkanWindow() noexcept {
    ImGui_ImplVulkanH_DestroyWindow(VulkanData::g_Instance,
                                    VulkanData::g_Device,
                                    &VulkanData::g_MainWindowData,
                                    VulkanData::g_Allocator);
}

VulkanSetuper::Status VulkanSetuper::SetupVulkan() noexcept {
    ImVector<const char *> extensions;
    uint32_t extensionsCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionsCount);
    for (uint32_t i = 0; i < extensionsCount; i++) {
        extensions.push_back(glfwExtensions[i]);
    }
    VkResult err;

    // Create Vulkan Instance
    {
        VkInstanceCreateInfo createInfo {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        };

        // Enumerate available extensions
        std::uint32_t propertiesCount;
        ImVector<VkExtensionProperties> properties;
        vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, nullptr);
        properties.resize(static_cast<int>(propertiesCount));
        err = vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, properties.Data);
        VulkanSetuper::CheckVkResult(err);

        // Enable required extensions
        if (IsExtensionAvailable(properties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
            extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        }
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
        if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) {
            extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        }
#endif

        // Enabling validation layers
#ifdef IMGUI_VULKAN_DEBUG_REPORT
        const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = layers;
        instance_extensions.push_back("VK_EXT_debug_report");
#endif

        // Create Vulkan Instance
        createInfo.enabledExtensionCount = static_cast<std::uint32_t>(extensions.Size);
        createInfo.ppEnabledExtensionNames = extensions.Data;
        err = vkCreateInstance(&createInfo,
                               VulkanData::g_Allocator,
                               &VulkanData::g_Instance);
        VulkanSetuper::CheckVkResult(err);

        // Setup the debug report callback
#ifdef IMGUI_VULKAN_DEBUG_REPORT
        auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(g_Instance, "vkCreateDebugReportCallbackEXT");
        IM_ASSERT(vkCreateDebugReportCallbackEXT != nullptr);
        VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
        debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        debug_report_ci.pfnCallback = debug_report;
        debug_report_ci.pUserData = nullptr;
        err = vkCreateDebugReportCallbackEXT(g_Instance, &debug_report_ci, g_Allocator, &g_DebugReport);
        check_vk_result(err);
#endif
    }

    // Select Physical Device (GPU)
    VulkanData::g_PhysicalDevice = SetupVulkan_SelectPhysicalDevice();

    // Select graphics queue family
    {
        std::uint32_t count;
        vkGetPhysicalDeviceQueueFamilyProperties(VulkanData::g_PhysicalDevice,
                                                 &count,
                                                 nullptr);
        auto queues = (VkQueueFamilyProperties *) malloc(sizeof(VkQueueFamilyProperties) * count);
        vkGetPhysicalDeviceQueueFamilyProperties(VulkanData::g_PhysicalDevice,
                                                 &count,
                                                 queues);
        for (std::uint32_t i = 0; i < count; i++) {
            if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                VulkanData::g_QueueFamily = i;
                break;
            }
        }
        free(queues);
        IM_ASSERT(VulkanData::g_QueueFamily != static_cast<std::uint32_t>(-1));
    }

    // Create Logical Device (with 1 queue)
    {
        ImVector<const char *> deviceExtensions;
        deviceExtensions.push_back("VK_KHR_swapchain");

        // Enumerate physical device extension
        std::uint32_t propertiesCount;
        ImVector<VkExtensionProperties> properties;
        vkEnumerateDeviceExtensionProperties(VulkanData::g_PhysicalDevice,
                                             nullptr,
                                             &propertiesCount,
                                             nullptr);
        properties.resize(static_cast<int>(propertiesCount));
        vkEnumerateDeviceExtensionProperties(VulkanData::g_PhysicalDevice,
                                             nullptr,
                                             &propertiesCount,
                                             properties.Data);
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
        if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
            deviceExtensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

        constexpr float queuePriority[] = { 1.0f };
        VkDeviceQueueCreateInfo queueInfo[1] = {};
        queueInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo[0].queueFamilyIndex = VulkanData::g_QueueFamily;
        queueInfo[0].queueCount = 1;
        queueInfo[0].pQueuePriorities = queuePriority;
        VkDeviceCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .queueCreateInfoCount = sizeof(queueInfo) / sizeof(queueInfo[0]),
                .pQueueCreateInfos = queueInfo,
                .enabledExtensionCount = static_cast<std::uint32_t>(deviceExtensions.Size),
                .ppEnabledExtensionNames = deviceExtensions.Data
        };
        err = vkCreateDevice(VulkanData::g_PhysicalDevice,
                             &createInfo,
                             VulkanData::g_Allocator,
                             &VulkanData::g_Device);
        VulkanSetuper::CheckVkResult(err);
        vkGetDeviceQueue(VulkanData::g_Device,
                         VulkanData::g_QueueFamily,
                         0,
                         &VulkanData::g_Queue);
    }

    // Create Descriptor Pool
    // The example only requires a single combined image sampler descriptor for the font image and only uses one descriptor set (for that)
    // If you wish to load e.g. additional textures you may need to alter pools sizes.
    {
        VkDescriptorPoolSize poolSizes[] = {
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
        };
        VkDescriptorPoolCreateInfo poolInfo {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
                .maxSets = 1,
                .poolSizeCount = static_cast<std::uint32_t>(IM_ARRAYSIZE(poolSizes)),
                .pPoolSizes = poolSizes
        };
        err = vkCreateDescriptorPool(VulkanData::g_Device,
                                     &poolInfo,
                                     VulkanData::g_Allocator,
                                     &VulkanData::g_DescriptorPool);
        VulkanSetuper::CheckVkResult(err);
    }
}

static void FrameRender(ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data) {
    VkResult err;

    VkSemaphore imageAcquiredSemaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
    VkSemaphore renderCompleteSemaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
    err = vkAcquireNextImageKHR(VulkanData::g_Device,
                                wd->Swapchain,
                                UINT64_MAX,
                                imageAcquiredSemaphore,
                                VK_NULL_HANDLE,
                                &wd->FrameIndex);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
        VulkanData::g_SwapChainRebuild = true;
        return;
    }
    VulkanSetuper::CheckVkResult(err);

    ImGui_ImplVulkanH_Frame *fd = &wd->Frames[wd->FrameIndex];
    {
        err = vkWaitForFences(VulkanData::g_Device,
                              1,
                              &fd->Fence,
                              VK_TRUE,
                              UINT64_MAX);    // wait indefinitely instead of periodically checking
        VulkanSetuper::CheckVkResult(err);

        err = vkResetFences(VulkanData::g_Device, 1, &fd->Fence);
        VulkanSetuper::CheckVkResult(err);
    }
    {
        err = vkResetCommandPool(VulkanData::g_Device, fd->CommandPool, 0);
        VulkanSetuper::CheckVkResult(err);
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
        VulkanSetuper::CheckVkResult(err);
    }
    {
        VkRenderPassBeginInfo info = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .renderPass = wd->RenderPass,
                .framebuffer = fd->Framebuffer,
                .renderArea.extent.width = static_cast<std::uint32_t>(wd->Width),
                .renderArea.extent.height = static_cast<std::uint32_t>(wd->Height),
                .clearValueCount = 1,
                .pClearValues = &wd->ClearValue
        };
        vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
    }

    // Record dear imgui primitives into command buffer
    ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

    // Submit command buffer
    vkCmdEndRenderPass(fd->CommandBuffer);
    {
        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo info = {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &imageAcquiredSemaphore,
                .pWaitDstStageMask = &waitStage,
                .commandBufferCount = 1,
                .pCommandBuffers = &fd->CommandBuffer,
                .signalSemaphoreCount = 1,
                .pSignalSemaphores = &renderCompleteSemaphore
        };

        err = vkEndCommandBuffer(fd->CommandBuffer);
        VulkanSetuper::CheckVkResult(err);
        err = vkQueueSubmit(VulkanData::g_Queue, 1, &info, fd->Fence);
        VulkanSetuper::CheckVkResult(err);
    }
}

VkPhysicalDevice VulkanSetuper::SetupVulkan_SelectPhysicalDevice() noexcept {
    std::uint32_t gpuCount;
    VkResult err = vkEnumeratePhysicalDevices(VulkanData::g_Instance, &gpuCount, nullptr);
    VulkanSetuper::CheckVkResult(err);
    IM_ASSERT(gpuCount > 0);

    ImVector<VkPhysicalDevice> gpus;
    gpus.resize(static_cast<int>(gpuCount));
    err = vkEnumeratePhysicalDevices(VulkanData::g_Instance, &gpuCount, gpus.Data);
    VulkanSetuper::CheckVkResult(err);

    // If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
    // most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
    // dedicated GPUs) is out of scope of this sample.
    for (VkPhysicalDevice &device : gpus) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            return device;
    }

    // Use first GPU (Integrated) is a Discrete one is not available.
    if (gpuCount > 0) {
        return gpus[0];
    }
    return VK_NULL_HANDLE;
}

bool VulkanSetuper::IsExtensionAvailable(const ImVector<VkExtensionProperties> &properties,
                                         const char *extension) noexcept {
    for (const VkExtensionProperties &p : properties) {
        if (strcmp(p.extensionName, extension) == 0) {
            return true;
        }
    }
    return false;
}

void VulkanSetuper::CheckVkResult(VkResult err) noexcept {
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}

void VulkanSetuper::GlfwErrorCallback(int error, const char *description) noexcept {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void VulkanSetuper::FrameRender(ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data) noexcept {
    VkResult err;

    VkSemaphore imageAcquiredSemaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
    VkSemaphore renderCompleteSemaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
    err = vkAcquireNextImageKHR(VulkanData::g_Device,
                                wd->Swapchain,
                                UINT64_MAX,
                                imageAcquiredSemaphore,
                                VK_NULL_HANDLE,
                                &wd->FrameIndex);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
        VulkanData::g_SwapChainRebuild = true;
        return;
    }
    VulkanSetuper::CheckVkResult(err);

    ImGui_ImplVulkanH_Frame *fd = &wd->Frames[wd->FrameIndex];
    {
        err = vkWaitForFences(VulkanData::g_Device, 1, &fd->Fence, VK_TRUE,
                              UINT64_MAX);    // wait indefinitely instead of periodically checking
        VulkanSetuper::CheckVkResult(err);

        err = vkResetFences(VulkanData::g_Device, 1, &fd->Fence);
        VulkanSetuper::CheckVkResult(err);
    }
    {
        err = vkResetCommandPool(VulkanData::g_Device, fd->CommandPool, 0);
        VulkanSetuper::CheckVkResult(err);
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
        VulkanSetuper::CheckVkResult(err);
    }
    {
        VkRenderPassBeginInfo info = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .renderPass = wd->RenderPass,
                .framebuffer = fd->Framebuffer,
                .renderArea.extent.width = static_cast<std::uint32_t>(wd->Width),
                .renderArea.extent.height = static_cast<std::uint32_t>(wd->Height),
                .clearValueCount = 1,
                .pClearValues = &wd->ClearValue
        };
        vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
    }

    // Record dear imgui primitives into command buffer
    ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

    // Submit command buffer
    vkCmdEndRenderPass(fd->CommandBuffer);
    {
        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo info {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &imageAcquiredSemaphore,
                .pWaitDstStageMask = &waitStage,
                .commandBufferCount = 1,
                .pCommandBuffers = &fd->CommandBuffer,
                .signalSemaphoreCount = 1,
                .pSignalSemaphores = &renderCompleteSemaphore
        };

        err = vkEndCommandBuffer(fd->CommandBuffer);
        VulkanSetuper::CheckVkResult(err);
        err = vkQueueSubmit(VulkanData::g_Queue, 1, &info, fd->Fence);
        VulkanSetuper::CheckVkResult(err);
    }
}

void VulkanSetuper::FramePresent(ImGui_ImplVulkanH_Window *wd) noexcept {
    if (VulkanData::g_SwapChainRebuild) {
        return;
    }
    VkSemaphore renderCompleteSemaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
    VkPresentInfoKHR info = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &renderCompleteSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &wd->Swapchain,
            .pImageIndices = &wd->FrameIndex
    };
    VkResult err = vkQueuePresentKHR(VulkanData::g_Queue, &info);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
        VulkanData::g_SwapChainRebuild = true;
        return;
    }
    VulkanSetuper::CheckVkResult(err);
    wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount; // Now we can use the next set of semaphores
}
