/*
 * Copyright 2012 Julian Maurice
 *
 * This file is part of lll.
 *
 * lll is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * lll is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with lll.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef lll_h_included
#define lll_h_included

#include <stdio.h>    // for FILE
#include <stdarg.h>   // for va_list

/*
 * DESCRIPTION
 *
 * liblll is a very simple log library designed for libraries.
 *
 * It provides wrapper functions around printf's family functions with a simple
 * templating system.
 * These functions are designed to be wrapped into macros or functions in your
 * own library.
 */

/*
 * TEMPLATE SPECIFICATION
 *
 * template parameter is a string describing the log format. It can contain the
 * followings special sequences of characters:
 *
 * %T    Current date and time (YYYY-mm-dd HH:MM:SS).
 *
 * %T{fmt}    Current date and time in format specified by fmt (see strftime).
 *
 * %p    Print PID.
 *
 * %P    Print PPID.
 *
 * %m    Print message passed in parameter. If ommited, the message is printed
 *       at the end of template.
 *       (See below to know how to pass extra parameters).
 *
 * $N    Where N is a digit between 0 and 9. Print the (N+1)th parameter.
 *       (See below to know how to pass extra parameters).
 *
 * ?N|if_true|if_false|    Where N is a digit between 0 and 9. Check if the
 *                         (N+1)th parameter is true (!= 0) and process if_true
 *                         if appropriate. Otherwise process if_false.
 *                         '|' delimiter can be any character. If delimiter is
 *                         present in if_true or if_false, it must be escaped
 *                         with '\'.
 *                         if_true and if_false can contain any special
 *                         sequences of characters allowed in template.
 *
 * \C    Where C is any character. Escape C so that it is not interpreted as a
 *       special character (print C as is).
 *       Note: backslash (\) have to appear twice in your string to be
 *       considered as a real backslash character. So you have to write "\\C".
 *
 * Note: a newline character will be automatically appended to template.
 */

/*
 * EXTRA PARAMETERS
 *
 * Extra parameters list is divided in three distinct parts:
 *   - a list of couple (fmt, value) where fmt is a string that contains a
 *   single conversion specification (see printf) which ends with the conversion
 *   specifier character (no trailing spaces allowed), and value is the value
 *   that will be printed using the given conversion specification.
 *   First couple corresponds to $0 in template, second parameter is $1, ...
 *   - two NULL parameters, to separate from other part.
 *   - message to print (%m) in printf format, followed by any needed parameters
 *
 * Example:
 *
 * int var = 42;
 * lll_print("%m at $0:$1", "%s", "main.c", "%d", 3, NULL, NULL, "var=%d", var);
 * will print: "var=42 at main.c:3"
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Print to stdout */
void lll_print(const char *template, ...);
void lll_vprint(const char *template, va_list va_ptr);
/* Print to file stream */
void lll_fprint(FILE *stream, const char *template, ...);
void lll_vfprint(FILE *stream, const char *template, va_list va_ptr);
/* Print to file path */
void lll_pprint(const char *path, const char *template, ...);
void lll_vpprint(const char *path, const char *template, va_list va_ptr);

#ifdef __cplusplus
}
#endif

#endif /* Not lll_h_included */

