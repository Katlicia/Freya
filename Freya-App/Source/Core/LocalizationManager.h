#ifndef LOCALIZATION_MANAGER_H
#define LOCALIZATION_MANAGER_H

#include <nlohmann/json.hpp>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#else
#include <locale.h>
#include <string.h>
#endif

class LocalizationManager {
public:
    bool Load(const std::string& filename);

    // Translation import function
    std::string Get(const std::string& key) const;

    static std::string GetSystemLanguage();

private:
    nlohmann::json m_Translations;  // JSON data structure
};

#endif // LOCALIZATION_MANAGER_H
