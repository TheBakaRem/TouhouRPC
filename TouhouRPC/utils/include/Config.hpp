#pragma once

#include <string>
#include <map>
#include <fstream>
#include <stdexcept>
#include <charconv>
#include <format>

#include "Log.hpp"

namespace Config {
    // Read the config file
    void parseFile(const std::string& path = "./config.ini");

    // Reads a value from the loaded configuration
    int getValue(const std::string& key);
};
