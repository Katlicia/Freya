#pragma once
#include <string>
#include "Tools/Canvas.h"
#include <fstream>

namespace FRE {

    class ProjectManager {
    public:
        ProjectManager(Canvas& canvas);

        bool SaveProject(const std::string& filePath);
        bool LoadProject(const std::string& filePath);
        static std::filesystem::path GetDefaultProjectDirectory();
        static std::string GenerateUniqueProjectName(const std::filesystem::path& dir, const std::string& baseName);

    private:
        Canvas& m_Canvas;
    };

}
