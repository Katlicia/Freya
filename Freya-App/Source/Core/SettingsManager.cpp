#include "SettingsManager.h"
#include <fstream>
#include <sstream>

using namespace FRE;


bool SettingsManager::Load(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) return false;

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty() || line[0] == '#') continue;
		auto delimiterPos = line.find('=');
		if (delimiterPos == std::string::npos) continue;

		std::string key = line.substr(0, delimiterPos);
		std::string value = line.substr(delimiterPos + 1);
		m_Settings[key] = value;
	}

	return true;
}

bool SettingsManager::Save(const std::string& filename) const {
	std::ofstream file(filename);
	if (!file.is_open()) return false;

	for (const auto& [key, value] : m_Settings) {
		file << key << "=" << value << "\n";
	}

	return true;
}

void SettingsManager::Set(const std::string& key, const std::string& value) {
	m_Settings[key] = value;
}

std::string SettingsManager::Get(const std::string& key, const std::string& defaultValue) const {
	auto it = m_Settings.find(key);
	return (it != m_Settings.end()) ? it->second : defaultValue;
}
