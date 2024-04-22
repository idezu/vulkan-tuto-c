#pragma once

#include "colors.h"

#define __PRINT_FILE_AND_LINE fprintf_color(stderr,COLOR_BLUE,__FILE__": %d: ",__LINE__)

#ifdef DEBUG

	/**
	 * @brief a macro for printing debug
	 */
	#define debug(str, ...) fprintf(stderr,"["); fprintf_color(stderr,COLOR_MAGENTA,"Verbose"); fprintf(stderr,"] "); __PRINT_FILE_AND_LINE; fprintf(stderr,str __VA_OPT__(,) __VA_ARGS__)
#else

	/**
	 * @brief expand to nothing 
	 */
	#define debug(...)
#endif

/**
 * @brief a macro for printing infos
 * 
 */
#define info( str, ... ) printf("["); printf_color(COLOR_GREEN,"Info"); printf("] "str __VA_OPT__(,) __VA_ARGS__)

/**
 * @brief a macro for printing warnings
 * 
 */
#define warning( str, ...) fprintf(stderr,"["); fprintf_color(stderr,COLOR_YELLOW,"Warning"); fprintf(stderr,"] "); __PRINT_FILE_AND_LINE; fprintf(stderr,str __VA_OPT__(,) __VA_ARGS__)

/**
 * @brief a macro for printing errors
 * 
 */
#define error( str, ...) fprintf(stderr,"["); fprintf_color(stderr,COLOR_RED,"Error"); fprintf(stderr,"] "); __PRINT_FILE_AND_LINE; fprintf(stderr,str __VA_OPT__(,) __VA_ARGS__)
