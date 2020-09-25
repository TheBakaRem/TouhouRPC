#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <map>

class Config
{
	// Singleton class used to parse and read configuration options.
	// Inspired from cppsecrets' C++ Config Reader.
public:
	/** Returns a unique instance of the object.
	  * Also responsible for the object construction.
	*/
	static Config* getInstance(); // Returns a unique instance of the class.

	/** Parses the config file given.
	  * Returns true if the file was succesfully parsed. Returns false otherwise.
	*/
	bool parseFile(std::string path = "./config.ini"); // Parse the config file given

	/** Gets the value from a given key parameter.
	  * Returns true if a value has been found, returns false otherwise
	*/
	bool getValue(std::string key, std::string& value);
	bool getValue(std::string key, int& value);

	/** Dumps the entire map in the specified ostream (cout by default)
	  * line format: key=value
	*/
	void dumpValues(std::ostream& ostream = std::cout);

private:
	std::map<std::string, std::string> configMap; // The config map obtained from the parsed config file
	static Config* p_instance; // Pointer to the current Config object
	
	// Private functions
	/** Takes a string and returns a trimmed version of this string.
	  * Useful when parsing config lines and for key/value pairs.
	*/
	void trim(std::string& str);

	// Private definitions to keep the class singleton
	Config(); // Default constructor
	~Config(); // Destructor
	Config(const Config& config) {} // Copy constructor
	Config& operator=(const Config& config) {} // Assignment operator
};

