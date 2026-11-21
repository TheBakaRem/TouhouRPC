#include "Log.hpp"

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