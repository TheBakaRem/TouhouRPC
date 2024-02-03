module;

#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <format>

export module Log;

namespace Log {
    using namespace std;

    export enum LogLevel {
        LOG_DEBUG = 0,
        LOG_INFO,
        LOG_WARNING,
        LOG_ERROR
    };

    // static internal variables must be declared here because they are used in the templates 
    // and we can't declare a template without an immediate definition
    const vector<string> level_names{ "DEBUG", "INFO", "WARNING", "ERROR" };
    ofstream log_file;
    LogLevel file_level{ LOG_ERROR };
    LogLevel console_level{ LOG_ERROR };

    auto local_time() {
        return chrono::floor<chrono::seconds>(chrono::zoned_time{ chrono::current_zone(), chrono::system_clock::now() }.get_local_time());
    }

    // Prints to console and log file at a specified level.
    // Only prints to a location if the level of the message is greater or equal to the location's log level.
    // Doesn't print to log file if the log file is closed.
    export template<typename... Args>
        void print(LogLevel level, const std::string& format_string, Args&&... args) {

        // We concatenate two formatted strings to avoid format string clashes like {0:} and {}. This way the caller of print() formats as they want.
        string message{
            /*vformat("[{0:%T}] {1:}: ", make_format_args(local_time() , level_names.at(level)))
            +*/ vformat(format_string, make_format_args(args...))
        };


        // Print in log file if it is opened
        if (log_file.is_open() && level >= file_level) {
            log_file << message << "\n";
        }

        // Print in console
        if (level >= console_level) {
            cout << message << "\n";
        }
    }

    // Opens a new log file.
    // If the logs folder doesn't exist, it is automatically created.
    // If a log file is already opened or if another error occurs, throws.
    export void openLogFile();

    export void setLogLevelFile(LogLevel level);

    export void setLogLevelConsole(LogLevel level);

    // Shorthand for print(Log::LOG_DEBUG)
    export template<typename... Args>
        void debug(Args&&... args) {
        print(LOG_DEBUG, args...);
    }

    // Shorthand for print(Log::LOG_INFO)
    export template<typename... Args>
        void info(Args&&... args) {
        print(LOG_INFO, args...);
    }

    // Shorthand for print(Log::LOG_WARNING)
    export template<typename... Args>
        void warning(Args&&... args) {
        print(LOG_WARNING, args...);
    }

    // Shorthand for print(Log::LOG_ERROR)
    export template<typename... Args>
        void error(Args&&... args) {
        print(LOG_ERROR, args...);
    }
};

module : private;

namespace Log {
    using namespace std;

    // strings can't be constexpr because char* implies heap allocation
    const string LOG_FOLDER_NAME{ "logs" };

    void openLogFile() {
        namespace fs = filesystem;

        if (log_file.is_open()) throw runtime_error("A log file is already opened in this instance.");

        const auto created_folder{ fs::create_directory(LOG_FOLDER_NAME) };

        const auto now{ local_time() };

        // We can't use %T because it inserts colons in the filename
        fs::path file_path{
            fs::current_path().append(
                vformat("{0:}\\thrpc_log {1:%Y-%m-%d} {1:%H-%M-%S}.txt", make_format_args(LOG_FOLDER_NAME, now))
            )
        };
        log_file.open(file_path);

        if (log_file.fail()) {
            throw runtime_error("Failed to open the log file at " + file_path.string() + ". Is the program allowed to create files ?");
        }

        log_file << "TOUHOURPC LOG FILE : local time " << now << "\n\n";
        if (created_folder) print(LOG_INFO, "Created logs directory.");
        cout << "Log file created! Logs for this session are available in " << file_path << "\n";
    }

    void setLogLevelFile(LogLevel level) {
        file_level = level;
    }

    void setLogLevelConsole(LogLevel level) {
        console_level = level;
    }
};