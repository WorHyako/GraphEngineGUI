#include "Project.hpp"

using namespace GEGui;

const std::filesystem::path &Project::getDirectory() const noexcept {
    return _directory;
}

void Project::load(std::filesystem::path rootPath) noexcept {
    _directory = std::move(rootPath);
}

