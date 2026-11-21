#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <format>


namespace Log {

    enum LogLevel {
        LOG_DEBUG = 0,
        LOG_INFO,
        LOG_WARNING,
        LOG_ERROR
    };

    // static internal variables must be declared here because they are used in the templates 
    // and we can't declare a template without an immediate definition
    const std::vector<std::string> level_names{ "DEBUG", "INFO", "WARNING", "ERROR" };
    inline std::ofstream log_file;
    inline LogLevel file_level{ LOG_ERROR };
    inline LogLevel console_level{ LOG_ERROR };

    inline auto local_time() {
        return std::chrono::floor<std::chrono::seconds>(std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::system_clock::now() }.get_local_time());
    }

    // Prints to console and log file at a specified level.
    // Only prints to a location if the level of the message is greater or equal to the location's log level.
    // Doesn't print to log file if the log file is closed.
    template<typename... Args>
        void print(LogLevel level, const std::string& format_string, Args&&... args) {

        // We concatenate two formatted strings to avoid format std::string clashes like {0:} and {}. This way the caller of print() formats as they want.
        std::string message{
            /*vformat("[{0:%T}] {1:}: ", make_format_args(local_time() , level_names.at(level)))
            +*/ std::vformat(format_string, std::make_format_args(args...))
        };


        // Print in log file if it is opened
        if (log_file.is_open() && level >= file_level) {
            log_file << message << "\n";
        }

        // Print in console
        if (level >= console_level) {
            std::cout << message << "\n";
        }
    }

    // Opens a new log file.
    // If the logs folder doesn't exist, it is automatically created.
    // If a log file is already opened or if another error occurs, throws.
    void openLogFile();

    void setLogLevelFile(LogLevel level);

    void setLogLevelConsole(LogLevel level);

    // Shorthand for print(Log::LOG_DEBUG)
    template<typename... Args>
        void debug(Args&&... args) {
        print(LOG_DEBUG, args...);
    }

    // Shorthand for print(Log::LOG_INFO)
    template<typename... Args>
        void info(Args&&... args) {
        print(LOG_INFO, args...);
    }

    // Shorthand for print(Log::LOG_WARNING)
    template<typename... Args>
        void warning(Args&&... args) {
        print(LOG_WARNING, args...);
    }

    // Shorthand for print(Log::LOG_ERROR)
    template<typename... Args>
        void error(Args&&... args) {
        print(LOG_ERROR, args...);
    }
};
