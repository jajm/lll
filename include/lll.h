/*
 * Copyright 2012 Julian Maurice
 *
 * This file is part of liblll.
 *
 * liblll is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liblll.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef lll_h_included
#define lll_h_included

#include <stdio.h>    // for FILE
#include <stdarg.h>   // for va_list

/* liblll is a very simple log library designed for libraries.
 *
 * It provides wrapper functions around printf's family functions with a simple
 * templating system.
 * These functions are designed to be wrapped into macros in your own library.
 * Below are explanations about each parameter. */

/* == Parameters specifications ==
 *
 * Only the first parameter is mandatory, all others can be NULL or 0 (with an
 * exception for va_ptr in lll_log).
 *
 *
 * -- template --
 *
 * template can contain:
 *
 * %T{fmt}: date and time, where fmt is in strftime format.
 *          Eg. "%T{%D %T}" prints with this format: "MM/DD/YY HH:MM:SS"
 *          fmt is optional, so you can specify %T alone. fmt will default to
 *          "%F %T" (YYYY-MM-DD HH:MM:SS)
 *
 * %d: domain (domain parameter)
 *
 * %l: log level (level parameter)
 *
 * %F: filename (filename parameter)
 *
 * %f: function name (function parameter)
 *
 * %L: line number (line parameter)
 *
 * %m: message (fmt parameter + additional parameters).
 *     This last one is optional and message will be appended to the end of
 *     output string if not specified.
 *
 * %%: '%' character
 *
 * Any other character is printed as is.
 *
 *
 * -- domain --
 *
 * A string that can be used to identify from which program/library the log is
 * coming (in case the library is used by multiple programs/libraries at the
 * same time.
 * Template: %d
 *
 *
 * -- current_level --
 *
 * Current log level of your program/library. Defines if a message will be
 * printed or not.
 *
 *
 * -- level --
 *
 * Level of message currently being printed. If greater than current_level,
 * message will not be printed.
 * Template: %l
 *
 *
 * -- filename --
 *
 * Name of file related to the message, if any.
 * Template: %F
 *
 *
 * -- function --
 *
 * Name of function related to the message, if any.
 * Template: %f
 *
 *
 * -- line --
 *
 * Line number in file.
 * Template: %L
 *
 *
 * -- fmt --
 *
 * Message to log, in printf-compatible format.
 *
 *
 * -- ... ---
 *
 * All other parameters are passed to vfprintf with fmt
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

void
lll_log_to_stream(
	FILE *stream,
	const char *template,
	const char *domain,
	int current_level,
	int level,
	const char *filename,
	const char *function,
	int line,
	const char *fmt,
	...
);

/* Log to file.
 * File is opened and closed each time this function is called. If you plan to
 * log very frequently and are concerned about performance, you should consider
 * using lll_log_to_stream instead. */
void
lll_log_to_file(
	const char *filepath,
	const char *template,
	const char *domain,
	int current_level,
	int level,
	const char *filename,
	const char *function,
	int line,
	const char *fmt,
	...
);

/* Functions above are just wrappers around this function.
 * You can use it directly if you have a va_list, but there is no check against
 * current log level (because it's not in parameters list), so you have to check
 * by yourself. */
void lll_log(
	FILE *stream,
	const char *template,
	const char *domain,
	int level,
	const char *filename,
	const char *function,
	int line,
	const char *fmt,
	va_list va_ptr
);

#ifdef __cplusplus
}
#endif

#endif /* Not lll_h_included */

