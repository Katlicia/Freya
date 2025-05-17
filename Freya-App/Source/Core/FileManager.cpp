#include "FileManager.h"
#include <algorithm>

namespace FRE {

    FileManager::FileManager(const LocalizationManager& langManager)
        : m_LanguageManager(langManager)
    {
        // Initialize with current directory
        m_CurrentPath = fs::current_path();
        m_SelectedPath = m_CurrentPath;
    }

    bool FileManager::ShowFolderSelectDialog(std::string& selectedPath) {
        m_DialogType = DialogType::SELECT_FOLDER;
        m_DialogOpen = true;
        bool result = DrawFileDialog(m_LanguageManager.Get("select_folder").c_str(), ImVec2(600, 400));

        if (result) {
            selectedPath = m_SelectedPath.string();
        }

        return result;
    }

    bool FileManager::ShowFileOpenDialog(std::string& selectedPath, const std::vector<std::string>& fileFilters) {
        m_DialogType = DialogType::OPEN_FILE;
        m_FileFilters = fileFilters;
        m_DialogOpen = true;
        bool result = DrawFileDialog(m_LanguageManager.Get("open_file").c_str(), ImVec2(600, 400));

        if (result) {
            selectedPath = m_SelectedPath.string();
        }

        return result;
    }

    bool FileManager::ShowFileSaveDialog(std::string& selectedPath, std::string& fileName, const std::vector<std::string>& fileFilters) {
        m_DialogType = DialogType::SAVE_FILE;
        m_FileFilters = fileFilters;
        m_FileName = fileName;
        m_DialogOpen = true;
        bool result = DrawFileDialog(m_LanguageManager.Get("save_file").c_str(), ImVec2(600, 400));

        if (result) {
            selectedPath = m_SelectedPath.string();
            fileName = m_FileName;
        }

        return result;
    }

    bool FileManager::IsFileMatchingFilters(const fs::path& file) {
        // If no filters specified, accept all files
        if (m_FileFilters.empty()) {
            return true;
        }

        std::string extension = file.extension().string();
        if (!extension.empty() && extension[0] == '.') {
            extension = extension.substr(1); // Remove leading dot
        }

        // Convert to lowercase for case-insensitive comparison
        std::transform(extension.begin(), extension.end(), extension.begin(),
            [](unsigned char c) { return std::tolower(c); });

        for (const auto& filter : m_FileFilters) {
            std::string filterLower = filter;
            std::transform(filterLower.begin(), filterLower.end(), filterLower.begin(),
                [](unsigned char c) { return std::tolower(c); });

            if (filterLower == extension) {
                return true;
            }
        }

        return false;
    }

    void FileManager::UpdateFileList(std::vector<fs::directory_entry>& folders, std::vector<fs::directory_entry>& files) {
        folders.clear();
        files.clear();

        try {
            for (const auto& entry : fs::directory_iterator(m_CurrentPath)) {
                if (entry.is_directory()) {
                    // Skip hidden folders if option is disabled
                    if (!m_ShowHiddenFiles && entry.path().filename().string()[0] == '.') {
                        continue;
                    }
                    folders.push_back(entry);
                }
                else if (entry.is_regular_file()) {
                    // Skip hidden files if option is disabled
                    if (!m_ShowHiddenFiles && entry.path().filename().string()[0] == '.') {
                        continue;
                    }

                    // For file open/save dialogs, check file filters
                    if (m_DialogType != DialogType::SELECT_FOLDER) {
                        if (IsFileMatchingFilters(entry.path())) {
                            files.push_back(entry);
                        }
                    }
                }
            }

            // Sort folders and files alphabetically
            std::sort(folders.begin(), folders.end(),
                [](const fs::directory_entry& a, const fs::directory_entry& b) {
                    return a.path().filename().string() < b.path().filename().string();
                });

            std::sort(files.begin(), files.end(),
                [](const fs::directory_entry& a, const fs::directory_entry& b) {
                    return a.path().filename().string() < b.path().filename().string();
                });
        }
        catch (const std::exception& e) {
            // Handle error: could not access directory
            // In a real app, you might show an error message or fall back to a default directory
            m_CurrentPath = fs::current_path(); // Fall back to current directory
        }
    }

    void FileManager::DrawPathNavigation() {
        // Back button
        if (ImGui::Button(m_LanguageManager.Get("back").c_str())) {
            if (m_CurrentPath.has_parent_path() && m_CurrentPath != m_CurrentPath.root_path()) {
                m_CurrentPath = m_CurrentPath.parent_path();
            }
        }

        ImGui::SameLine();

        // Home button (go to user home directory)
        if (ImGui::Button(m_LanguageManager.Get("home").c_str())) {
            m_CurrentPath = fs::path(getenv("USERPROFILE")); // Windows
            // For cross-platform, check if directory exists and fall back:
            // if (!fs::exists(m_CurrentPath)) m_CurrentPath = fs::path(getenv("HOME")); // Unix-like
        }

        ImGui::SameLine();

        // Current path display (with navigation capability)
        std::string currentPathStr = m_CurrentPath.string();
        ImGui::Text("%s: ", m_LanguageManager.Get("current_path").c_str());
        ImGui::SameLine();

        // Make the path interactive by splitting into segments
        fs::path pathForSegments = m_CurrentPath;
        std::vector<fs::path> segments;

        while (pathForSegments != pathForSegments.root_path() && !pathForSegments.empty()) {
            segments.push_back(pathForSegments);
            pathForSegments = pathForSegments.parent_path();
        }

        // Add root
        if (!segments.empty() && segments.back().has_root_path()) {
            segments.push_back(segments.back().root_path());
        }

        // Reverse to get proper order
        std::reverse(segments.begin(), segments.end());

        // Display segments as clickable buttons
        for (size_t i = 0; i < segments.size(); ++i) {
            std::string segmentName;

            if (i == 0) {
                // Root path
                segmentName = segments[i].string();
            }
            else {
                segmentName = segments[i].filename().string();
                if (segmentName.empty()) { // For paths like C:\
                                    segmentName = segments[i].string();
                }
            }

            if (ImGui::SmallButton(segmentName.c_str())) {
                m_CurrentPath = segments[i];
                break;
            }

            if (i < segments.size() - 1) {
                ImGui::SameLine();
                ImGui::Text("/");
                ImGui::SameLine();
            }
        }
    }

    bool FileManager::DrawFileDialog(const char* title, ImVec2 size) {
        bool result = false;

        if (!m_DialogOpen) return false;

        ImGui::OpenPopup(title);

        // Set position to center of window
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(size);

        bool open = true;
        if (ImGui::BeginPopupModal(title, &open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings)) {
            // Path navigation section
            DrawPathNavigation();

            ImGui::Separator();

            // File/folder list section
            std::vector<fs::directory_entry> folders;
            std::vector<fs::directory_entry> files;
            UpdateFileList(folders, files);

            // Special parent folder entry
            if (m_CurrentPath != m_CurrentPath.root_path() && m_CurrentPath.has_parent_path()) {
                if (ImGui::Selectable("..")) {
                    m_CurrentPath = m_CurrentPath.parent_path();
                }
            }

            // Folders first
            for (const auto& folder : folders) {
                std::string folderName = folder.path().filename().string();

                if (ImGui::Selectable((std::string(" ") + folderName).c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                    if (ImGui::IsMouseDoubleClicked(0)) {
                        // Double-click to navigate into folder
                        m_CurrentPath = folder.path();
                    }
                    else {
                        // Single click selects the folder (for directory selection mode)
                        if (m_DialogType == DialogType::SELECT_FOLDER) {
                            m_SelectedPath = folder.path();
                            m_FileName = "";
                        }
                    }
                }
            }

            // Then files
            if (m_DialogType != DialogType::SELECT_FOLDER) {
                for (const auto& file : files) {
                    std::string fileName = file.path().filename().string();

                    if (ImGui::Selectable((std::string(" ") + fileName).c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                        m_SelectedPath = m_CurrentPath;
                        m_FileName = fileName;

                        if (ImGui::IsMouseDoubleClicked(0)) {
                            // Double-click to select and confirm
                            result = true;
                            m_DialogOpen = false;
                            ImGui::CloseCurrentPopup();
                        }
                    }
                }
            }

            ImGui::Separator();

            // File name input field for save dialog
            static char filenameBuffer[256] = "";

            if (m_DialogType == DialogType::SAVE_FILE) {
                // For save dialog, show filename input
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%s", m_LanguageManager.Get("file_name").c_str());
                ImGui::SameLine();

                if (m_FileName.length() > 0) {
                    strncpy(filenameBuffer, m_FileName.c_str(), sizeof(filenameBuffer));
                }

                if (ImGui::InputText("##FileName", filenameBuffer, sizeof(filenameBuffer))) {
                    m_FileName = filenameBuffer;
                }
            }
            else if (m_DialogType == DialogType::SELECT_FOLDER) {
                // For folder selection, show current path
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%s", m_LanguageManager.Get("selected_folder").c_str());
                ImGui::SameLine();

                // If a folder is selected, show it, otherwise show current path
                std::string selectedFolderStr = m_SelectedPath.empty() ? m_CurrentPath.string() : m_SelectedPath.string();
                ImGui::TextWrapped("%s", selectedFolderStr.c_str());
            }

            // Buttons at the bottom
            ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 40);
            ImGui::Separator();

            if (m_DialogType == DialogType::SELECT_FOLDER) {
                if (ImGui::Button(m_LanguageManager.Get("use_current_folder").c_str(), ImVec2(200, 0))) {
                    m_SelectedPath = m_CurrentPath;
                    result = true;
                    m_DialogOpen = false;
                    ImGui::CloseCurrentPopup();
                }
            }

            // Action button (Open/Save)
            ImGui::SameLine();
            if (ImGui::Button(
                m_DialogType == DialogType::SAVE_FILE ? m_LanguageManager.Get("save").c_str() :
                m_DialogType == DialogType::OPEN_FILE ? m_LanguageManager.Get("open").c_str() :
                m_LanguageManager.Get("select").c_str(),
                ImVec2(120, 0))) {

                // For save dialog, require a filename
                if (m_DialogType == DialogType::SAVE_FILE && m_FileName.empty()) {
                    // Show error: filename required
                }
                else {
                    // For folder selection, use current path if nothing selected
                    if (m_DialogType == DialogType::SELECT_FOLDER && m_SelectedPath.empty()) {
                        m_SelectedPath = m_CurrentPath;
                    }
                    else if (m_DialogType != DialogType::SELECT_FOLDER) {
                        // For file operations, use current path + filename
                        m_SelectedPath = m_CurrentPath / m_FileName;
                    }

                    result = true;
                    m_DialogOpen = false;
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::SameLine();

            if (ImGui::Button(m_LanguageManager.Get("cancel").c_str(), ImVec2(120, 0))) {
                m_DialogOpen = false;
                ImGui::CloseCurrentPopup();
            }

            // Handle close request
            if (!open) {
                m_DialogOpen = false;
            }

            ImGui::EndPopup();
        }
        else {
            m_DialogOpen = false;
        }

        return result;
    }

} 