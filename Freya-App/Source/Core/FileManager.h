#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "LocalizationManager.h"

namespace fs = std::filesystem;

namespace FRE {

    class FileManager {
    public:
        FileManager(const LocalizationManager& langManager);
        ~FileManager() = default;

        bool ShowFolderSelectDialog(std::string& selectedPath);
        bool ShowFileOpenDialog(std::string& selectedPath, const std::vector<std::string>& fileFilters = {});
        bool ShowFileSaveDialog(std::string& selectedPath, std::string& fileName, const std::vector<std::string>& fileFilters = {});

    private:
        const LocalizationManager& m_LanguageManager;

        fs::path m_CurrentPath;
        fs::path m_SelectedPath;
        std::string m_FileName;
        std::vector<std::string> m_FileFilters;
        bool m_ShowHiddenFiles = false;
        bool m_DialogOpen = false;

        enum class DialogType {
            OPEN_FILE,
            SAVE_FILE,
            SELECT_FOLDER
        };

        DialogType m_DialogType = DialogType::OPEN_FILE;

        void UpdateFileList(std::vector<fs::directory_entry>& folders, std::vector<fs::directory_entry>& files);
        bool IsFileMatchingFilters(const fs::path& file);
        void DrawPathNavigation();
        bool DrawFileDialog(const char* title, ImVec2 size);
    };

}