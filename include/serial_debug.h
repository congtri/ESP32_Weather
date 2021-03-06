/*
 * serial_debug.h
 *
 *  Created on: Jun 1, 2018
 *      Author: truongcongtri
 */

#ifndef APP_INC_SERIAL_DEBUG_H_
#define APP_INC_SERIAL_DEBUG_H_

/* Define SERIAL_DEBUG */
#define ENABLE_SLOG_DEBUG
#define ENABLE_SLOG_WARN
#define ENABLE_SLOG_ERROR
#define ENABLE_SLOG_INFO

/* Using tiny printf library */
#ifdef TINY_PRINTF
#include "tinyprintf.h"
#define SERIAL_LOG(...) tfp_printf(__VA_ARGS__)
#else
/* Using GCC retarget printf library */
#define SERIAL_LOG(...) printf(__VA_ARGS__);
#endif

/* Define log info */
#ifdef ENABLE_SLOG_DEBUG
#define SLOG_DEBUG(...)          \
	do                           \
	{                            \
		SERIAL_LOG("\r\n");      \
		SERIAL_LOG("[DEBUG]: "); \
		SERIAL_LOG(__VA_ARGS__); \
	} while (0);
#else
#define SLOG_DEBUG(...) \
	do                  \
	{                   \
	} while (0);
#endif

/* Define log info */
#ifdef ENABLE_SLOG_ERROR
#define SLOG_ERROR(...)          \
	do                           \
	{                            \
		SERIAL_LOG("\r\n");      \
		SERIAL_LOG("[ERROR]: "); \
		SERIAL_LOG(__VA_ARGS__); \
	} while (0);
#else
#define SLOG_ERROR(...) \
	do                  \
	{                   \
	} while (0);
#endif

/* Define log info */
#ifdef ENABLE_SLOG_WARN
#define SLOG_WARN(...)           \
	do                           \
	{                            \
		SERIAL_LOG("\r\n");      \
		SERIAL_LOG("[WARN ]: "); \
		SERIAL_LOG(__VA_ARGS__); \
	} while (0);
#else
#define SLOG_WARN(...) \
	do                 \
	{                  \
	} while (0);
#endif

/* Define log info */
#ifdef ENABLE_SLOG_INFO
#define SLOG_INFO(...)           \
	do                           \
	{                            \
		SERIAL_LOG("[INFO ]: "); \
		SERIAL_LOG(__VA_ARGS__); \
		SERIAL_LOG("\r\n");      \
	} while (0);
#else
#define SLOG_INFO(...) \
	do                 \
	{                  \
	} while (0);
#endif

#endif /* APP_INC_SERIAL_DEBUG_H_ */
