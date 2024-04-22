#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>

#if defined(_WIN32) || defined(WIN32) 
# include <windows.h>

# define COLOR_RED FOREGROUND_RED
# define COLOR_GREEN   FOREGROUND_GREEN
# define COLOR_YELLOW FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN
# define COLOR_BLUE    FOREGROUND_BLUE
# define COLOR_MAGENTA FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE
# define COLOR_CYAN FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN

HANDLE hConsole;
CONSOLE_SCREEN_BUFFER_INFO consoleInfo;


/**
 * @file
 * @brief everything that is color related
 */

/**
 * @brief a function to print in a certain color
 * @param[in] stream the file to print to
 * @param[in] color COLOR_RED, COLOR_GREEN, ...
 * @param[in] message the message
 * @result 
 */
static int fprintf_color(FILE* restrict stream, int color, const char * message, ...)
	{
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		WORD saved_attributes;

		/* Save current attributes */
		GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
		saved_attributes = consoleInfo.wAttributes;

		SetConsoleTextAttribute(hConsole, color);
		va_list args;
		va_start(args , message);
		int r = vfprintf(file, message, args);
		va_end(args);

    		/* Restore original attributes */
    		SetConsoleTextAttribute(hConsole, saved_attributes);
		return (r > 0);
	}

static int printf_color(int color, const char *message, ...)
	{
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		WORD saved_attributes;

		/* Save current attributes */
		GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
		saved_attributes = consoleInfo.wAttributes;

		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
		va_list args;
		va_start(args , message);
		int r = vprintf(message, args);
		va_end(args);

    		/* Restore original attributes */
    		SetConsoleTextAttribute(hConsole, saved_attributes);
		return (r > 0);
	}

#elif defined(__unix__) || defined(linux)

# define COLOR_RED     "\x1b[31m"
# define COLOR_GREEN   "\x1b[32m"
# define COLOR_YELLOW  "\x1b[33m"
# define COLOR_BLUE    "\x1b[34m"
# define COLOR_MAGENTA "\x1b[35m"
# define COLOR_CYAN    "\x1b[36m"
# define COLOR_RESET   "\x1b[0m"

# define printf_color(color, message,...) printf(color message COLOR_RESET __VA_OPT__(,) __VA_ARGS__)
# define fprintf_color(stream,color, message,...) fprintf(stream, color message COLOR_RESET __VA_OPT__(,) __VA_ARGS__)

#endif// defined(_WIN32) || defined(WIN32) 

#endif
