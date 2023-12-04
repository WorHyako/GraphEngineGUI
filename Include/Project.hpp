#pragma once

#include <filesystem>

namespace GEGui {

    /**
     *
     */
    class Project {
    protected:
        /**
         * Ctor
         */
        Project() = default;

        /**
         * Dtor
         */
        virtual ~Project() = default;

    public:
        /**
         *
         */
         void load(std::filesystem::path rootPath) noexcept;

        /**
         *
         * @return
         */
        [[nodiscard]] const std::filesystem::path &getDirectory() const noexcept;

    private:

        /**
         *
         */
        std::filesystem::path _directory;

        /**
         *
         */
        static Project activeProject;
    };
}
