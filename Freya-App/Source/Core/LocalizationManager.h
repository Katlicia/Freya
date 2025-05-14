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
    // Yükleme fonksiyonu
    bool Load(const std::string& filename);

    // Çeviri alma fonksiyonu
    std::string Get(const std::string& key) const;

    // Sistem dilini alma fonksiyonu
    static std::string GetSystemLanguage();

private:
    nlohmann::json m_Translations;  // JSON veri yapýsý
};

#endif // LOCALIZATION_MANAGER_H
