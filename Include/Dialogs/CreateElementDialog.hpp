#pragma once

#include <filesystem>

namespace GEGui::Dialogs {

    /**
     *
     */
    enum class FileType : std::uint8_t {
        /**
         *
         */
        File,

        /**
         *
         */
        Folder
    };

    /**
     *
     * @author Wor Hyako
     */
    class CreateElementDialog {
    public:
        /**
         *
         * @return
         */
        void open(std::string dialogName, std::filesystem::path directoryPath, FileType fileType) noexcept;

        /**
         *
         */
        void close() noexcept;

    private:
        std::filesystem::path filePath {};
    };
}
