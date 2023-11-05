#pragma once

#include "imgui_impl_vulkan.h"

namespace GEGui::ApiSetup {

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

//#define IMGUI_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define IMGUI_VULKAN_DEBUG_REPORT
#endif

#ifdef IMGUI_VULKAN_DEBUG_REPORT
    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
    (void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
    fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
    return VK_FALSE;
}
#endif

    /**
     *
     */
    class VulkanSetuper {
    public:

        /**
         *
         * @param instance_extensions
         */
        static void SetupVulkan(ImVector<const char *> instance_extensions) noexcept;

        /**
         * All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
         * Your real engine/app may not use them.
         * @param wd
         * @param surface
         * @param width
         * @param height
         */
        static void SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd,
                                      VkSurfaceKHR surface,
                                      int width,
                                      int height) noexcept;

        /**
         *
         */
        static void CleanupVulkan() noexcept;

        /**
         *
         */
        static void CleanupVulkanWindow() noexcept;

        /**
         *
         * @return
         */
        static VkPhysicalDevice SetupVulkan_SelectPhysicalDevice() noexcept;

        /**
         *
         * @param properties
         * @param extension
         * @return
         */
        static bool IsExtensionAvailable(const ImVector<VkExtensionProperties> &properties,
                                         const char *extension) noexcept;

        /**
         *
         * @param err
         */
        static void CheckVkResult(VkResult err) noexcept;

        /**
         *
         * @param error
         * @param description
         */
        static void GlfwErrorCallback(int error, const char *description) noexcept;

        /**
         *
         * @param wd
         * @param draw_data
         */
        static void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data) noexcept;

        /**
         *
         * @param wd
         */
        static void FramePresent(ImGui_ImplVulkanH_Window* wd) noexcept;
    };
}
