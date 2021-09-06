#ifndef __LOG_H__
#define __LOG_H__

#include "string.h"

#if DEBUG >= 4
#define debug(msg, ...) printf("[debug] %s:%i " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug(msg, ...)
#endif

#if DEBUG >= 3
#define info(msg, ...) printf("[info] " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define info(msg, ...)
#endif

#if DEBUG >= 1
#define warn(msg, ...) printf("[warn] %s:%i " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define warn(msg, ...)
#endif

#if DEBUG >= 0
#define error(msg, ...) printf("[error] %s:%i " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define error(msg, ...)
#endif
#endif
