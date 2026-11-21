#include "Log.hpp"

#include <filesystem>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>

namespace Log {

    // strings can't be constexpr because char* implies heap allocation
    const std::string LOG_FOLDER_NAME{ "logs" };

    void openLogFile() {
        namespace fs = std::filesystem;

        if (log_file.is_open()) {
            throw std::runtime_error("A log file is already opened in this instance.");
        }

        const auto created_folder{ fs::create_directory(LOG_FOLDER_NAME) };

        const auto now{ local_time() };

        // We can't use %T because it inserts colons in the filename
        const fs::path file_path{
            fs::current_path().append(
                format("{0:}\\thrpc_log {1:%Y-%m-%d} {1:%H-%M-%S}.txt", LOG_FOLDER_NAME, now)
            )
        };
        log_file.open(file_path);

        if (log_file.fail()) {
            throw std::runtime_error("Failed to open the log file at " + file_path.string() + ". Is the program allowed to create files ?");
        }

        log_file << "TOUHOURPC LOG FILE : local time " << now << "\n\n";
        if (created_folder) { print(LogLevel::LOG_INFO, "Created logs directory."); }
        std::cout << "Log file created! Logs for this session are available in " << file_path << "\n";
    }

    void setLogLevelFile(LogLevel level) {
        file_level = level;
    }

    void setLogLevelConsole(LogLevel level) {
        console_level = level;
    }
}  // namespace Log