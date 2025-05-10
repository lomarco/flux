#pragma once

#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...)                                                  \
  do {                                                                         \
    fprintf(stderr, "DEBUG [%s:%d]: " fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
  } while (0)
#else
#define DEBUG_PRINT(fmt, ...) \
  do {                        \
  } while (0)
#endif
