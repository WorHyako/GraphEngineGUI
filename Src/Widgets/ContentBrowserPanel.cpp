#include "Widgets/ContentBrowserPanel.hpp"
#include "Project.hpp"
#include "Style/Icons/IconIDs.hpp"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include "WorLibrary/TemplateWrapper/Singleton.hpp"
#include "Dialogs/CreateElementDialog.hpp"

#include <iostream>
#include <fstream>

using namespace GEGui::Widgets;
using namespace Wor::TemplateWrapper;

ContentBrowserPanel::ContentBrowserPanel() noexcept {
    _baseDirectory = std::filesystem::current_path() += std::filesystem::path("/Content");
    _currentDirectory = _baseDirectory;
}

void ContentBrowserPanel::render() noexcept {
    ImGui::Begin("Content Browser");

    ImGui::Columns(2, nullptr, true);
    ImGui::SetColumnWidth(0, 90);

    if (ImGui::Button("New file", ImVec2(90, 30))) {
        ImGui::OpenPopup("Create new file dialog");
    }

    ImGui::NextColumn();
    ImGui::SetColumnWidth(0, 90);

    if (ImGui::Button("New folder", ImVec2(90, 30))) {
        ImGui::OpenPopup("Create new folder dialog");
    }

    ImGui::SameLine();

    if (_currentDirectory != std::filesystem::path(_baseDirectory)) {
        if (ImGui::Button("<-")) {
            _currentDirectory = _currentDirectory.parent_path();
        }
    }

    float panelWidth = ImGui::GetContentRegionAvail().x;
    float cellSize = 128.0f + 16.0f;

    int columnCount = static_cast<int>(panelWidth / cellSize);
    if (columnCount < 1) {
        columnCount = 1;
    }

    Dialogs::CreateElementDialog createFileDialog;
    createFileDialog.open("Create new file dialog", _currentDirectory, Dialogs::FileType::File);

    Dialogs::CreateElementDialog createFolderDialog;
    createFileDialog.open("Create new folder dialog", _currentDirectory, Dialogs::FileType::Folder);

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

        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space)) && ImGui::IsItemHovered()) {
            std::cout << "\b space ";
        }
        if (ImGui::BeginPopupContextItem(nullptr)) {
            if (ImGui::MenuItem("Delete")) {
                std::filesystem::remove(path);
            }

            if (ImGui::MenuItem("Rename")) {
            }

            ImGui::EndPopup();
        }

        static std::filesystem::path draggingFile;
        if (ImGui::BeginDragDropSource()) {
            draggingFile = path;
            auto *itemPath = reinterpret_cast<const wchar_t *>(path.c_str());
            ImGui::SetDragDropPayload("File dragging", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (ImGui::AcceptDragDropPayload("File dragging")) {
                std::filesystem::copy(draggingFile, path, std::filesystem::copy_options::overwrite_existing);
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

    ImGui::End();
}
