#pragma once

#include "imgui.h"

#include "Controls/MainWindow.hpp"

#include <memory>

struct GLFWwindow;

namespace GEGui::ApiSetup {

    /**
     *
     */
    class ImGuiSetup {
    public:
        /**
         *
         */
        void Init(const Controls::MainWindow& window) noexcept;

        /**
         *
         */
        void Create() noexcept;

        /**
         *
         */
        void SetupColor() noexcept;

    private:
        /**
         *
         */
        std::unique_ptr<ImGuiContext*> context;

    };
}
