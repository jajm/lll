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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TIME_BUFFER_SIZE 128
#define TIME_FMT_BUFFER_SIZE 64

const char * lll_log_time(FILE *stream, const char *start)
{
	char buffer[TIME_BUFFER_SIZE];
	char fmt[TIME_FMT_BUFFER_SIZE];
	time_t t;
	struct tm *tm;
	int i = 0;
	const char *ptr = start;

	if (*(ptr+1) == '{') {
		ptr += 2;
		while (i+1 < TIME_FMT_BUFFER_SIZE && *ptr != '}'
		&& *ptr != '\0') {
			fmt[i++] = *ptr;
			ptr++;
		}

		if (*ptr == '\0') {
			fputs("ERROR: lll reached the end of template string "
				"without finding closing brace for %T\n",
				stderr);
			return start;
		} else if (*ptr != '}') {
			fprintf(stderr, "ERROR: lll can't handle string longer "
				"than %d characters for time format\n",
				TIME_FMT_BUFFER_SIZE);
			return start;
		}

		fmt[i] = '\0';
	} else {
		/* Default format YYYY-MM-DD HH:MM:SS */
		strcpy(fmt, "%F %T");
	}

	t = time(NULL);
	tm = localtime(&t);
	strftime(buffer, TIME_BUFFER_SIZE, fmt, tm);
	fputs(buffer, stream);

	return ptr;
}

void lll_log(FILE *stream, const char *template, const char *domain, int level,
	const char *filename, const char *function, int line, const char *fmt,
	va_list va_ptr)
{
	const char *c;
	int message_printed = 0;
	
	if (template == NULL) {
		/* Just print fmt and return */
		vfprintf(stream, fmt, va_ptr);
		fputc('\n', stream);
		return;
	}

	c = template;
	while (*c != '\0') {
		if (*c == '%') {
			c++;
			switch (*c) {
				case '%':
					fputc('%', stream);
					break;

				/* Time */
				case 'T':
					c = lll_log_time(stream, c);
					break;

				/* Domain */
				case 'd':
					if (domain != NULL) {
						fputs(domain, stream);
					}
					break;

				/* Level */
				case 'l':
					fprintf(stream, "%d", level);
					break;

				/* Filename */
				case 'F':
					if (filename != NULL) {
						fputs(filename, stream);
					}
					break;

				/* Function */
				case 'f':
					if (function != NULL) {
						fputs(function, stream);
					}
					break;

				/* Line */
				case 'L':
					fprintf(stream, "%d", line);
					break;

				/* Message */
				case 'm':
					vfprintf(stream, fmt, va_ptr);
					message_printed = 1;
					break;
				case '\0':
					c--;
				default:
					fputc('%', stream);
					fputc(*c, stream);
			}
		} else {
			fputc(*c, stream);
		}
		c++;
	}

	if (!message_printed) {
		/* If %m is not in template, print message at the end */
		vfprintf(stream, fmt, va_ptr);
	}
	fputc('\n', stream);
}

void lll_log_to_stream(FILE *stream, const char *template, const char *domain,
	int current_level, int level, const char *filename,
	const char *function, int line, const char *fmt, ...)
{
	va_list va_ptr;

	if (level <= current_level) {
		va_start(va_ptr, fmt);
		lll_log(stream, template, domain, level, filename, function,
			line, fmt, va_ptr);
		va_end(va_ptr);
	}
}

void lll_log_to_file(const char *filepath, const char *template,
	const char *domain, int current_level, int level, const char *filename,
	const char *function, int line, const char *fmt, ...)
{
	va_list va_ptr;
	FILE *stream;

	if (level <= current_level) {
		stream = fopen(filepath, "a");
		if (stream != NULL) {
			va_start(va_ptr, fmt);
			lll_log(stream, template, domain, level, filename,
				function, line, fmt, va_ptr);
			va_end(va_ptr);
			fclose(stream);
		} else {
			fprintf(stderr, "Failed to open %s in append mode",
				filepath);
			perror(" ");
		}
	}
}

