#include "Log.h"

// Default static values

FILE* Log::p_logFile = nullptr;
Log* Log::p_instance = nullptr;
bool Log::logFileOpened = false;
int Log::logLevelConsole = Log::LOG_INFO;
int Log::logLevelLogFile = Log::LOG_INFO;

const char* Log::logLevelNames[]{ "DEBUG", "INFO", "WARNING", "ERROR" }; // Log level names
const wchar_t Log::logFolderName[] = L"logs";							 // Folder name
const char Log::logFileNameFormat[] = "logs/thrpc_log_%d%02d%02d-%02d%02d%02d.txt";	// Name style: thrpc_log_yyyymmdd-hhmmss.txt


// CONSTRUCTOR
Log::Log() {}

// DESTRUCTOR
Log::~Log() {}

// Get instance (used for object creation and to keep the class as singleton)
Log* Log::getInstance() {
	if (p_instance == nullptr) p_instance = new Log();
	return p_instance;
}

// Prints to console and log file at a specified level.
void Log::print(int level, const char* message, ...) const {
	
	va_list args;
	va_start(args, message);

	time_t currTime = time(NULL);
	struct tm currTm;
	localtime_s(&currTm, &currTime);

	// Print in log file if it is opened
	if (logFileOpened && level >= logLevelLogFile) {
		fprintf_s(p_logFile, "[%02d:%02d:%02d] %s: ", currTm.tm_hour, currTm.tm_min, currTm.tm_sec, logLevelNames[level]);
		vfprintf_s(p_logFile, message, args);
		fprintf_s(p_logFile, "\n");
	}

	// Print in console
	if (level >= logLevelConsole) {
		fprintf_s(stdout, "[%02d:%02d:%02d] %s: ", currTm.tm_hour, currTm.tm_min, currTm.tm_sec, logLevelNames[level]);
		vfprintf_s(stdout, message, args);
		fprintf_s(stdout, "\n");
	}

	va_end(args);
}

// Open log file
int Log::openLogFile() {
	if (logFileOpened) return LOGFILE_ALREADY_OPENED; // Don't recreate another log file if not needed.

	// Create a logs folder / Check if it already exists
	if (CreateDirectory(logFolderName, NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {

		// Create the log file itself
		char logFileName[50];

		time_t currTime = time(NULL);
		struct tm currTm;
		localtime_s(&currTm, &currTime);

		sprintf_s(logFileName, logFileNameFormat, currTm.tm_year + 1900, currTm.tm_mon + 1, currTm.tm_mday, currTm.tm_hour, currTm.tm_min, currTm.tm_sec); // Format log file name
		fopen_s(&p_logFile, logFileName, "w");

		if (p_logFile != NULL) {
			std::cout << "Log file created! Logs for this session are available in " << logFileName << std::endl;

			fprintf_s(p_logFile, "TOUHOURPC LOG FILE : %02d/%02d/%d %02d:%02d:%02d\n\n", currTm.tm_mday, currTm.tm_mon + 1, currTm.tm_year + 1900, currTm.tm_hour, currTm.tm_min, currTm.tm_sec);

			logFileOpened = true;

			return LOGFILE_SUCCESS;
		}
		else {
			std::cout << "An error occured while creating the log file." << std::endl;
		}
	}
	else {
		std::cout << "An error occured while creating the log folder. Windows error code: " << GetLastError() << std::endl;
		std::cout << "Is the program allowed to create folders where it's located?" << std::endl;
	}

	std::cout << "The program will continue running, but the displayed logs won't be saved in a log file." << std::endl;
	return LOGFILE_ERROR;
}

// Close log file
int Log::closeLogFile() {
	if (!logFileOpened) return LOGFILE_ALREADY_CLOSED; // Don't close log file if it's not opened.

	time_t currTime = time(NULL);
	struct tm currTm;
	localtime_s(&currTm, &currTime);

	fprintf_s(p_logFile, "\nLOG CLOSED :  %02d/%02d/%d %02d:%02d:%02d\n", currTm.tm_mday, currTm.tm_mon + 1, currTm.tm_year + 1900, currTm.tm_hour, currTm.tm_min, currTm.tm_sec);

	if (fclose(p_logFile)) {
		// Error in fclose
		std::cout << "An error occured while closing the log file." << std::endl;
		return LOGFILE_ERROR;
	}

	p_logFile = NULL;
	logFileOpened = 0;

	std::cout << "Log file has been closed." << std::endl;
	return LOGFILE_SUCCESS;
}

// Set the console's log level
void Log::setLogLevelConsole(int level) {
	if (level < LOG_DEBUG) logLevelConsole = LOG_DEBUG;
	else if (level > LOG_ERROR) logLevelConsole = LOG_ERROR;
	else logLevelConsole = level;
}

//Set the log file's log level
void Log::setLogLevelLogFile(int level) {
	if (level < LOG_DEBUG) logLevelLogFile = LOG_DEBUG;
	else if (level > LOG_ERROR) logLevelLogFile = LOG_ERROR;
	else logLevelLogFile = level;
}

// Global definition
Log* logSystem = Log::getInstance();