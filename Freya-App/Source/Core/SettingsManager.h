#pragma once
#include <string>
#include <unordered_map>

namespace FRE {

	class SettingsManager {
	public:

		bool Load(const std::string& filename);
		bool Save(const std::string& filename) const;

		void Set(const std::string& key, const std::string& value);
		std::string Get(const std::string& key, const std::string& defaultValue = "") const;

	private:
		std::unordered_map<std::string, std::string> m_Settings;
	};

}
