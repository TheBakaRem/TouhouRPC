#include "Config.hpp"

namespace Config {

    // Private map where the loaded configuration is stored
    std::map<const std::string, const int> configMap{
        {"activateLogFiles", 1},
        {"logLevelConsole", Log::LOG_INFO},
        {"logLevelLogFile", Log::LOG_INFO}
    };

    // std::string custom trim
    void trim(std::string& str) {
        // Trim left
        str.erase(0, str.find_first_not_of(" \n\t\r"));
        // Trim right
        str.erase(str.find_last_not_of(" \n\t\r") + 1);
    }

    // Read the config file
    void parseFile(const std::string& path) {
        std::ifstream inputFile;
        inputFile.open(path);

        if (inputFile.fail()) {
            throw std::runtime_error("Error while loading the config file at " + path);
        }

        std::string line;
        while (std::getline(inputFile, line)) {
            trim(line);

            // Check empty and comment lines (starts with ";")
            // Sections are considered comment lines (starts with "[" and ends with "]")
            if (line.size() == 0 || line.find_first_of(";[") == 0) continue;

            // Get key/value data
            const auto delim_pos{ line.find("=") };
            std::string key;
            std::string value;

            if (delim_pos != std::string::npos) {
                key = line.substr(0, delim_pos);
                value = line.substr(delim_pos + 1);

                trim(key);
                trim(value);
            }

            // If key or value is empty, we skip
            if (key.empty() || value.empty()) continue;

            // Replace the old value if it exist, we don't bother checking if the new value is different
            const auto it{ configMap.find(key) };
            if (it != configMap.end()) {
                configMap.erase(key);
            }

            // convert the read value to int
            int int_value;
            const auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), int_value);

            if (ec == std::errc()) {
                configMap.insert({ key, int_value });
            }
            else if (ec == std::errc::invalid_argument) {
                throw std::runtime_error("Error while reading the config file at " + path + ": the value of " + key + " = " + value + " is not an int.");
            }
            else if (ec == std::errc::result_out_of_range) {
                throw std::runtime_error("Error while reading the config file at " + path + ": the value of " + key + " = " + value + " is too big to be read as int.");
            }
        }
    }

    int getValue(const std::string& key) {
        const auto it{ configMap.find(key) };
        if (it != configMap.end()) {
            return it->second;
        }

        throw std::runtime_error("No value found in the config for key " + key);
    }
};