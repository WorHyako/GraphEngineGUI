#pragma once

#include <filesystem>

namespace GEGui::Widgets {

    class ContentBrowserPanel {
    public:
        /**
         * Ctor
         */
        ContentBrowserPanel() noexcept;

        /**
         *
         */
        void Render() noexcept;

    private:
        /**
         *
         */
        std::filesystem::path _baseDirectory;

        /**
         *
         */
        std::filesystem::path _currentDirectory;
    };
}
