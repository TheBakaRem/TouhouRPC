#include "Log.h"

// Globals

const char* logLevelNames[]{ "DEBUG", "INFO", "WARNING", "ERROR" }; // Log level names

static FILE* logFile = NULL;
static int logLevel = LOG_INFO;										// Minimum level required to print the log to the console / log file.
static wchar_t logFolderName[] = L"logs";							// Folder name

static char logFileNameFormat[] = "logs/thrpc_log_%d%02d%02d-%02d%02d%02d.txt";	// Name style: thrpc_log_yyyymmdd-hhmmss.txt

static int isOpened = 0;	// Check if log file is opened or not (to not open two files at the same time)

// Print log
void printLog(int level, const char* string, ...) {
	if (level >= logLevel)
	{

		va_list args;
		va_start(args, string);

		time_t currTime = time(NULL);
		struct tm currTm;
		localtime_s(&currTm, &currTime);
		
		// Print in log file if it is opened
		if (isOpened) {
			fprintf_s(logFile, "[%02d:%02d:%02d] %s: ", currTm.tm_hour, currTm.tm_min, currTm.tm_sec, logLevelNames[level]);
			vfprintf_s(logFile, string, args);
			fprintf_s(logFile, "\n");
		}

		// Print in console
		fprintf_s(stdout, "[%02d:%02d:%02d] %s: ", currTm.tm_hour, currTm.tm_min, currTm.tm_sec, logLevelNames[level]);
		vfprintf_s(stdout, string, args);
		fprintf_s(stdout, "\n");
		
		va_end(args);
	}
}



// Set log level
void setLogLevel(int level) {
	if (level < LOG_DEBUG) logLevel = LOG_DEBUG;
	else if (level > LOG_ERROR) logLevel = LOG_ERROR;
	else logLevel = level;
}

// Log file initialization
int logInit() {
	if (isOpened == 1) return -2; // Don't create another log instance.

	// Create a logs folder / Check if it already exists
	if (CreateDirectory(logFolderName, NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
		char logFileName[50];

		time_t currTime = time(NULL);
		struct tm currTm;
		localtime_s(&currTm, &currTime);

		sprintf_s(logFileName, logFileNameFormat, currTm.tm_year + 1900, currTm.tm_mon + 1, currTm.tm_mday, currTm.tm_hour, currTm.tm_min, currTm.tm_sec); // Format log file name
		fopen_s(&logFile, logFileName, "w");

		if (logFile != NULL) {
			std::cout << "Log file created! Logs for this session are available in " << logFileName << std::endl;
			
			fprintf_s(logFile, "TOUHOURPC LOG FILE : %02d/%02d/%d %02d:%02d:%02d\n\n", currTm.tm_mday, currTm.tm_mon + 1, currTm.tm_year + 1900, currTm.tm_hour, currTm.tm_min, currTm.tm_sec);

			isOpened = 1;

			return 0;
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
	return -1;
}

// Log file closing
void logExit() {
	if (isOpened == 0) return; // Don't exit log if it's not needed.

	time_t currTime = time(NULL);
	struct tm currTm;
	localtime_s(&currTm, &currTime);

	fprintf_s(logFile, "\nLOG CLOSED :  %02d/%02d/%d %02d:%02d:%02d\n", currTm.tm_mday, currTm.tm_mon + 1, currTm.tm_year + 1900, currTm.tm_hour, currTm.tm_min, currTm.tm_sec);
	std::cout << "Log file has been closed." << std::endl;

	fclose(logFile);
	logFile = NULL;
	isOpened = 0;
}