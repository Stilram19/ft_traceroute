#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

// @brief logs the error message and exits with the exit status in 'status' argument
void   errorLogger(char *message, int status);

// @brief displays messages for debugging
void   debugLogger(char *message);

// @brief displays general info messages
void   infoLogger(char *message);

// @brief displays ping results
void   resultLogger(char *result);

#endif
