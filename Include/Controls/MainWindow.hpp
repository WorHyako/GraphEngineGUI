#pragma once

#include "ApiSetup/VulkanSetuper.hpp"

#include "WorLibrary/Math/Vector/Vector2.hpp"

#include <memory>

struct GLFWwindow;

namespace GEGui::Controls {

    /**
     *
     */
    class MainWindow {
    public:
        /**
         * Ctor
         */
        MainWindow() noexcept;

        /**
         *
         */
        enum class Status : std::uint8_t {
            /**
             *
             */
            Success,

            /**
             * Default starting condition
             */
            ZeroCheck,

            /**
             * Error on glfw initialization
             */
            GlfwInitError,

            /**
             * GLFW: Vulkan Not Supported
             */
            VulkanNotSupported,
        };

        /**
         *
         */
        [[nodiscard]] bool Init() noexcept;

        /**
         *
         */
        void CreateWindowSurface() noexcept;

    private:
        /**
         *
         */
        std::shared_ptr<GLFWwindow *> _window;

        /**
         *
         */
        Status _status;

        /**
         *
         */
        VkSurfaceKHR _surface;

    public:
#pragma region Accessors

        [[nodiscard]] GLFWwindow* GetWindow() const noexcept;

        [[nodiscard]] const VkSurfaceKHR &GetSurface() const noexcept;

        [[nodiscard]] Wor::Vector::Vector2<int> GetFrameSize() const noexcept;

        [[nodiscard]] Status GetStatus() const noexcept;

#pragma endregion Accessors
    };
}
