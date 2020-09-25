#include "Config.h"

// Default p_instance value
Config* Config::p_instance = nullptr;

// CONSTRUCTOR
Config::Config() {
	configMap.clear();
}

// DESTRUCTOR
Config::~Config() {
	configMap.clear();
}

// Get instance (used for object creation and to keep the class as singleton)
Config* Config::getInstance() {
	if (p_instance == nullptr) p_instance = new Config();
	return p_instance;
}

// Parse config file
bool Config::parseFile(std::string path) {
	std::ifstream inputFile;
	inputFile.open(path);

	// Check is file is opened
	if (inputFile.fail()) {
		return false;
	}
	
	// Parsing
	std::string line;
	while (std::getline(inputFile, line)) {

		trim(line);

		// Check empty and comment lines (starts with ";")
		// Sections are considered comment lines (starts with "[" and ends with "]")
		if (line.size() == 0 || line.find_first_of(";[") == 0) continue;

		// Get key/value data
		size_t delim_pos = line.find("=");
		std::string key, value = "";

		if (delim_pos != std::string::npos) {
			key = line.substr(0, delim_pos);
			value = line.substr(delim_pos+1);

			trim(key);
			trim(value);
		}

		// If key or value is empty, we skip
		if (key.empty() || value.empty()) continue;

		// Check if the key already exists in the map. If it does, we erase it and save the latest key/value pair instead.
		std::map<std::string, std::string>::iterator it = configMap.find(key);
		if (it != configMap.end()) {
			configMap.erase(key);
		}
		configMap.insert(std::pair <std::string, std::string>(key, value));
	}
	
	// End of parsing
	return true;
}

// Get value from specific key given. Returns true if a value has been found, returns false otherwise.
// String value
bool Config::getValue(std::string key, std::string& value) {
	std::map<std::string, std::string>::iterator it = configMap.find(key);
	if (it != configMap.end() && it->second != "") {
		value = it->second;
		return true;
	}

	// No value has been found, sets a default empty string
	value = "";
	return false;
}

// Integer value
bool Config::getValue(std::string key, int& value) {
	std::map<std::string, std::string>::iterator it = configMap.find(key);
	if (it != configMap.end() && it->second != "") {
		value = std::atoi((it->second).c_str());
		return true;
	}

	// No value has been found, sets a default -1 value
	value = -1;
	return false;
}

// String trim
void Config::trim(std::string& str)
{
	// Trim left
	str.erase(0, str.find_first_not_of(" \n\t\r"));

	// Trim right
	str.erase(str.find_last_not_of(" \n\t\r") + 1);
}

// Dumps the saved map into ostream
void Config::dumpValues(std::ostream& ostream) {
	std::map<std::string, std::string>::iterator it;
	ostream << "==========CONFIG DUMP==========" << std::endl;
	for (it = configMap.begin(); it != configMap.end(); ++it) {
		ostream << it->first << "=" << it->second << std::endl;
	}
	ostream << "===============================" << std::endl;
}


