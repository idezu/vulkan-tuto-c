#pragma once

#include "colors.h"

#define __PRINT_FILE_AND_LINE                                                  \
  fprintf_color(stderr, COLOR_BLUE, __FILE__ ": %d: ", __LINE__)
#define __PRINT_FILE_LINE_FN                                                   \
  fprintf_color(stderr, COLOR_BLUE, __FILE__ ": %d: %s(): ", __LINE__, __func__)

#ifdef DEBUG

/**
 * @brief a macro for printing debug
 */
#define debug(str, ...)                                                        \
  do {                                                                         \
    fprintf(stderr, "[");                                                      \
    fprintf_color(stderr, COLOR_MAGENTA, "Verbose");                           \
    fprintf(stderr, "] ");                                                     \
    __PRINT_FILE_AND_LINE;                                                     \
    fprintf(stderr, str __VA_OPT__(, ) __VA_ARGS__);                           \
  } while (0)
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
#define info(str, ...)                                                         \
  do {                                                                         \
    printf("[");                                                               \
    printf_color(COLOR_GREEN, "Info");                                         \
    printf("] ");                                                              \
    printf(str __VA_OPT__(, ) __VA_ARGS__);                                    \
  } while (0)

/**
 * @brief a macro for printing warnings
 *
 */
#define warning(str, ...)                                                      \
  do {                                                                         \
    fprintf(stderr, "[");                                                      \
    fprintf_color(stderr, COLOR_YELLOW, "Warning");                            \
    fprintf(stderr, "] ");                                                     \
    __PRINT_FILE_AND_LINE;                                                     \
    fprintf(stderr, str __VA_OPT__(, ) __VA_ARGS__);                           \
  } while (0)

/**
 * @brief a macro for printing errors
 *
 */
#define error(str, ...)                                                        \
  do {                                                                         \
    fprintf(stderr, "[");                                                      \
    fprintf_color(stderr, COLOR_RED, "Error");                                 \
    fprintf(stderr, "] ");                                                     \
    __PRINT_FILE_LINE_FN;                                                      \
    fprintf(stderr, str __VA_OPT__(, ) __VA_ARGS__);                           \
  } while (0)

#define CHECK(code, ...)                                                       \
  do {                                                                         \
    if ((code) != VK_SUCCESS) {                                                \
      __VA_OPT__(error(__VA_ARGS__);)                                          \
      return (code);                                                           \
    }                                                                          \
  } while (0)
