#pragma once

#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <iostream>


// Enums
enum { LOG_DEBUG = 0, LOG_INFO, LOG_WARNING, LOG_ERROR}; // Different log levels

enum { LOGINIT_SUCCESS = 0, LOGINIT_ERROR = -1, LOGINIT_ALREADY_OPENED = 1}; // logInitializeLogFile return values

// Functions

/** Print to the log file and the console with a specified logging level. 
  * String formatting is the same as printf.
  * If the log file isn't initialized, we only print to the console.
*/
void printLog(int level, const char* string, ...);

/** Set the logging level for terminal.
  * Logs with a level strictly lower than the currently set level will not be displayed.
*/
void setLogLevelConsole(int level);

/** Set the logging level for log files.
  * Logs with a level strictly lower than the currently set level will not be displayed.
*/
void setLogLevelLogFile(int level);

/** Initialize the log system.
  * if logFile is set to true, a log file will be created at the same time.
  * if not, the display will only go to the console.
*/
void logInitialize(bool logFile = true);

/** Initializes the log file.
  * Returns 0 if the log file is created.
  * Returns 1 if a log file is already opened (nothing is changed in that case).
  * Returns -1 if the log file / log folder creation caused an error.
*/
int logInitializeLogFile();

/** Closes the log file.
*/
void logExit();