#pragma once

#include <string>

namespace Config {
    //! @brief Read the config file
    void parseFile(const std::string& path = "./config.ini");

    //! @brief Reads a value from the loaded configuration
    int getValue(const std::string& key);
};
