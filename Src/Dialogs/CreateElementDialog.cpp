#include "Dialogs/CreateElementDialog.hpp"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include <fstream>
#include <utility>

using namespace GEGui::Dialogs;

void CreateElementDialog::open(std::string dialogName,
                               std::filesystem::path directoryPath,
                               FileType fileType) noexcept {
    if (!ImGui::BeginPopupModal(dialogName.c_str(),
                                nullptr,
                                ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse)) {
        return;
    }
    std::string elementName;
    switch (fileType) {
        case FileType::File:
            elementName = "file";
            break;
        case FileType::Folder:
            elementName = "folder";
            break;
        default:
            break;
    }
    filePath = std::move(directoryPath);
    ImGui::Text("%s", std::string("Enter new " + elementName + " file's name").c_str());

    ImGui::Separator();

    static std::string fileName { elementName };

    ImGui::InputText("##", &fileName, ImGuiInputTextFlags_EnterReturnsTrue);

    const auto newElementPath = filePath / fileName;
    const auto pathExist = std::filesystem::exists(newElementPath);
    if (pathExist) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
        ImGui::Text("%s", std::string(elementName + " already exist!").c_str());
        ImGui::PopStyleColor();
    }

    if (ImGui::Button("Apply", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Enter) && !pathExist) {
        switch (fileType) {
            case FileType::File: {
                std::ofstream ofs { newElementPath.string() };
                ofs.close();
                break;
            }
            case FileType::Folder:
                std::filesystem::create_directory(newElementPath);
                break;
        }
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

void CreateElementDialog::close() noexcept {
    ImGui::CloseCurrentPopup();
}
