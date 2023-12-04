#include "Widgets/ContentBrowserPanel.hpp"
#include "Project.hpp"
#include "Style/Icons/IconIDs.hpp"

#include <imgui.h>

#include "WorLibrary/TemplateWrapper/Singleton.hpp"

#include <cstdio>
#include <iostream>

using namespace GEGui::Widgets;
using namespace Wor::TemplateWrapper;


ContentBrowserPanel::ContentBrowserPanel() noexcept {
    _baseDirectory = std::filesystem::current_path() += std::filesystem::path("/Content");
    _currentDirectory = _baseDirectory;
}

void ContentBrowserPanel::Render() noexcept {
    ImGui::Begin("Content Browser");

    if (_currentDirectory != std::filesystem::path(_baseDirectory)) {
        if (ImGui::Button("<-")) {
            _currentDirectory = _currentDirectory.parent_path();
        }
    }

    if (ImGui::BeginPopupContextItem(nullptr)) {

        if (ImGui::MenuItem("Delete")) {}

        ImGui::EndPopup();
    }
    float panelWidth = ImGui::GetContentRegionAvail().x;
    float cellSize = 128.0f + 16.0f;

    int columnCount = static_cast<int>(panelWidth / cellSize);
    if (columnCount < 1) {
        columnCount = 1;
    }

    ImGui::Columns(columnCount, nullptr, true);

    for (const auto &directory : std::filesystem::directory_iterator(_currentDirectory)) {
        const auto &path = directory.path();
        const std::string filename = path.filename().string();

        ImGui::PushID(filename.c_str());
        const auto icon = directory.is_directory()
                          ? Style::Icons::IconFolder
                          : Style::Icons::IconFile;
        std::string buttonText = '\n' + std::string(icon) + '\n' + filename;
        if (ImGui::Button(buttonText.c_str(), ImVec2(150, 80))) {
        }

        if (ImGui::BeginPopupContextItem(nullptr)) {
            if (ImGui::MenuItem("Delete")) {
                std::remove(path.c_str());
            }

            if (ImGui::MenuItem("Rename")) {
            }
            ImGui::EndPopup();
        }

        static std::filesystem::path draggingFile;
        if (ImGui::BeginDragDropSource()) {
            draggingFile = path;
            auto *itemPath = reinterpret_cast<const wchar_t *>(path.c_str());
            ImGui::SetDragDropPayload("FILES", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (ImGui::AcceptDragDropPayload("FILES")) {
                std::cout << "\npath: " << path;
                std::cout << "\ndragging file: " << draggingFile;
                std::filesystem::copy(draggingFile, path, std::filesystem::copy_options::overwrite_existing);
                std::cout << "\ndragging file: " << draggingFile;
                std::filesystem::remove(draggingFile);
            }
            ImGui::EndDragDropTarget();
        }
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            if (directory.is_directory()) {
                _currentDirectory /= path.filename();
            }
        }
        ImGui::NextColumn();

        ImGui::PopID();
    }

    ImGui::Columns(1);

    ImGui::End();
}
