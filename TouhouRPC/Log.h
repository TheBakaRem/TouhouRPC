#pragma once

#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <iostream>

class Log {
    // Singleton class used to setup the log files and prints to console/log files.
public:
    // Enums
    /* Log levels */
    enum { LOG_DEBUG = 0, LOG_INFO, LOG_WARNING, LOG_ERROR };

    /* openLogFile / closeLogFile return values */
    enum { LOGFILE_SUCCESS = 0, LOGFILE_ERROR = -1, LOGFILE_ALREADY_OPENED = 1, LOGFILE_ALREADY_CLOSED = 2 };


    /** Returns a unique instance of the object.
      * Also responsible for the object construction.
    */
    static Log* getInstance();

    /** Prints to console and log file at a specified level.
      * String formatting is the same as printf.
      * Only prints to a location if the level of the message is greater or equal to the location's log level.
      * Doesn't print to log file if the log file is closed.
    */
    void print(int level, const char* message, ...) const;

    /** Opens a new log file and returns LOGFILE_SUCCESS (0) on success.
      * If the logs folder doesn't exist, it is automatically created.
      * If a log file is already opened, returns LOGFILE_ALREADY_OPENED (1)
      * If another error occured, returns LOGFILE_ERROR (-1)
    */
    int openLogFile();

    /** Closes the currently opened log file and returns LOGFILE_SUCCESS (0) on success
      * If the log file is already closed or was never opened, returns LOGFILE_ALREADY_CLOSED (2)
      * If another error occured, returns LOGFILE_ERROR (-1)
    */
    int closeLogFile();

    /** Sets the log level for the console
    */
    void setLogLevelConsole(int level);

    /** Sets the log level for the log file
    */
    void setLogLevelLogFile(int level);



private:
    static Log* p_instance; // Pointer to the current Log object
    static bool logFileOpened; // Check if a log file is opened
    static FILE* p_logFile; // Pointer to the log file
    static int logLevelConsole; // Current console log level
    static int logLevelLogFile; // Current log file log level

    static const char* logLevelNames[];    // Log level names (used for display)
    static const wchar_t logFolderName[];  // Folder name where logs are stored
    static const char logFileNameFormat[]; // Log filename format (to keep log names similar)

    // Private definitions to keep the class singleton
    Log(); // Default constructor
    ~Log(); // Destructor
    Log(const Log& log) {} // Copy constructor
    Log& operator=(const Log& log) {} // Assignment operator
};


// Global instance pointer
extern Log* logSystem;