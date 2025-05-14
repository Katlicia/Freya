// LocalizationManager.cpp

#include "LocalizationManager.h"
#include <fstream>

bool LocalizationManager::Load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    file >> m_Translations;
    return true;
}

std::string LocalizationManager::Get(const std::string& key) const {
    auto it = m_Translations.find(key);
    if (it != m_Translations.end())
        return it->get<std::string>();
    return key;
}

std::string LocalizationManager::GetSystemLanguage() {
#ifdef _WIN32
    LANGID langId = GetUserDefaultUILanguage();
    switch (PRIMARYLANGID(langId)) {
    case LANG_TURKISH: return "tr";
    case LANG_ENGLISH: return "en";
    default: return "en";
    }
#else
    const char* locale = setlocale(LC_ALL, "");
    if (locale && strstr(locale, "tr")) return "tr";
    return "en";
#endif
}
