#include "Controls/MainWindow.hpp"

#include "ApiSetup/VulkanData.hpp"

#include "imgui_impl_glfw.h"
#include "GLFW/glfw3.h"

#include <vulkan/vulkan.h>

using namespace GEGui::Controls;

MainWindow::MainWindow() noexcept
        : _status(Status::ZeroCheck),
          _surface(nullptr) {
}

bool MainWindow::Init() noexcept {
    glfwSetErrorCallback(ApiSetup::VulkanSetuper::GlfwErrorCallback);
    if (!glfwInit()) {
        _status = Status::GlfwInitError;
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+Vulkan example", nullptr, nullptr);
    _window = std::make_shared<GLFWwindow *>(window);
    if (!glfwVulkanSupported()) {
        _status = Status::VulkanNotSupported;
        return false;
    }
    _status = Status::Success;
    return true;
}

void MainWindow::CreateWindowSurface() noexcept {
    VkResult err = glfwCreateWindowSurface(ApiSetup::VulkanData::g_Instance,
                                           *_window,
                                           ApiSetup::VulkanData::g_Allocator,
                                           &_surface);
    ApiSetup::VulkanSetuper::CheckVkResult(err);
}

#pragma region Accessors

GLFWwindow* MainWindow::GetWindow() const noexcept {
    return *_window;
}

MainWindow::Status MainWindow::GetStatus() const noexcept {
    return _status;
}

const VkSurfaceKHR &MainWindow::GetSurface() const noexcept {
    return _surface;
}

Wor::Vector::Vector2<int> MainWindow::GetFrameSize() const noexcept {
    Wor::Vector::Vector2<int> size;
    glfwGetFramebufferSize(*_window, &size.x, &size.y);
    return size;
}

#pragma endregion Accessors
