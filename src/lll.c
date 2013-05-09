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

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* for strftime */
#include <time.h>

/* for getpid, getppid */
#include <sys/types.h>
#include <unistd.h>

typedef enum {INTEGER, DOUBLE, LONGDOUBLE} param_type_t;

typedef struct {
	param_type_t type;
	union {
		intptr_t i;
		double d;
		long double ld;
	} data;
	const char *format;
} parameter_t;

param_type_t guess_param_type(const char *fmt)
{
	const char *c = fmt;
	int longdouble = 0;
	param_type_t type;

	/* Check the conversion specifier to guess the type */
	while (*(c+1) != '\0') {
		if (*c == 'L') {
			longdouble = 1;
		}
		c++;
	}
	switch (*c) {
		case 'e':case 'E':case 'f':case 'F':
		case 'g':case 'G':case 'a':case 'A':
			if (longdouble) {
				type = LONGDOUBLE;
			} else {
				type = DOUBLE;
			}
			break;
		default:
			type = INTEGER;
	}

	return type;
}

unsigned int build_params(parameter_t params[10], va_list va_ptr)
{
	unsigned int i = 0;
	const char *fmt;

	while (i < 10 && (fmt = va_arg(va_ptr, const char *)) != NULL) {
		param_type_t type = guess_param_type(fmt);
		switch (type) {
			case INTEGER:
				params[i].data.i = va_arg(va_ptr, intptr_t);
				break;
			case DOUBLE:
				params[i].data.d = va_arg(va_ptr, double);
				break;
			case LONGDOUBLE:
				params[i].data.ld = va_arg(va_ptr, long double);
				break;
		}
		params[i].format = fmt;
		params[i].type = type;
		i++;
	}
	while (i < 10) {
		params[i].type = INTEGER;
		params[i].data.i = 0;
		params[i].format = NULL;
		i++;
	}

	return i;
}

typedef struct {
	char *buf;
	unsigned pos;
	size_t size;
} lll_buffer_t;

void lll_buffer_init(lll_buffer_t *b)
{
	b->size = BUFSIZ;
	b->pos = 0;
	b->buf = calloc(b->size, sizeof(char));
}

void lll_buffer_free(lll_buffer_t *b)
{
	free(b->buf);
}

void lll_buffer_ensure_large_enough(lll_buffer_t *b, size_t length)
{
	while (b->size - b->pos < length + 1) {
		b->size = b->size * b->size;
	}
	b->buf = realloc(b->buf, b->size);
}

void bputc(char c, lll_buffer_t *b)
{
	lll_buffer_ensure_large_enough(b, 1);
	b->buf[b->pos++] = c;
	b->buf[b->pos] = '\0';
}

void bputs(const char *s, lll_buffer_t *b)
{
	while (*s != '\0') {
		bputc(*s, b);
		s++;
	}
}

void vbprintf(lll_buffer_t *b, const char *fmt, va_list va_ptr)
{
	int count;

	if (fmt != NULL) {
		lll_buffer_ensure_large_enough(b, strlen(fmt));
		count = vsprintf(b->buf + b->pos, fmt, va_ptr);
		b->pos += count;
	}
}

void bprintf(lll_buffer_t *b, const char *fmt, ...)
{
	va_list va_ptr;

	va_start(va_ptr, fmt);
	vbprintf(b, fmt, va_ptr);
	va_end(va_ptr);
}

#define TIME_BUFFER_SIZE 128
#define TIME_FMT_BUFFER_SIZE 64

void process_time(lll_buffer_t *buffer, const char **tpl_cursor)
{
	char buf[TIME_BUFFER_SIZE];
	char fmt[TIME_FMT_BUFFER_SIZE];
	time_t t;
	struct tm *tm;
	int i = 0;
	const char *ptr = *tpl_cursor;
	int valid = 0;

	if (*ptr == '%' && *(ptr+1) == 'T') {
		ptr++;
		valid = 1;
		if (*(ptr+1) == '{') {
			ptr+=2;
			while (i+1 < TIME_FMT_BUFFER_SIZE && *ptr != '}'
			&& *ptr != '\0') {
				fmt[i++] = *ptr;
				ptr++;
			}
			fmt[i] = '\0';

			if (*ptr == '\0' || *ptr != '}') {
				valid = 0;
			}
		} else {
			/* Default format YYYY-MM-DD HH:MM:SS */
			strcpy(fmt, "%F %T");
		}
	}

	if (valid) {
		t = time(NULL);
		tm = localtime(&t);
		strftime(buf, TIME_BUFFER_SIZE, fmt, tm);
		bputs(buf, buffer);

		*tpl_cursor = ptr;
	}
}

void process_pid(lll_buffer_t *buffer, const char **tpl_cursor)
{
	const char *ptr = *tpl_cursor;
	pid_t pid;

	if (*ptr == '%' && *(ptr+1) == 'p') {
		pid = getpid();
		bprintf(buffer, "%u", pid);
		*tpl_cursor = ptr+1;
	}
}

void process_ppid(lll_buffer_t *buffer, const char **tpl_cursor)
{
	const char *ptr = *tpl_cursor;
	pid_t ppid;

	if (*ptr == '%' && *(ptr+1) == 'P') {
		ppid = getppid();
		bprintf(buffer, "%u", ppid);
		*tpl_cursor = ptr+1;
	}
}

void process_special(lll_buffer_t *buffer, const char **tpl_cursor, const char *fmt,
	va_list va_ptr, int *mprinted)
{
	const char *ptr = *tpl_cursor;

	if (*ptr == '%') {
		switch (*(ptr+1)) {
			case 'T':
				process_time(buffer, &ptr);
				break;
			case 'p':
				process_pid(buffer, &ptr);
				break;
			case 'P':
				process_ppid(buffer, &ptr);
				break;
			case 'm':
				vbprintf(buffer, fmt, va_ptr);
				ptr++;
				*mprinted = 1;
				break;
			case '%':
				bputc('%', buffer);
				ptr++;
				break;
		}
		*tpl_cursor = ptr;
	}

}

void process_parameter(lll_buffer_t *buffer, const char **tpl_cursor,
	parameter_t params[10])
{
	const char *ptr = *tpl_cursor;
	parameter_t *p;

	if (*ptr == '$') {
		ptr++;
		if (*ptr >= '0' && *ptr <= '9') {
			p = &params[*ptr - '0'];
			switch (p->type) {
				case INTEGER:
					bprintf(buffer, p->format, p->data.i);
					break;
				case DOUBLE:
					bprintf(buffer, p->format, p->data.d);
					break;
				case LONGDOUBLE:
					bprintf(buffer, p->format, p->data.ld);
					break;
			}
			*tpl_cursor = ptr;
		}
	}
}

void skip_template_block(const char **tpl_cursor)
{
	const char *ptr = *tpl_cursor;

	if (*ptr != '\0') {
		char delim = *ptr;
		ptr ++;
		while (*ptr != '\0' && !(*ptr == delim && *(ptr-1) != '\\')) {
			if (*ptr == '?') {
				if (*(ptr+1) != '\0' && *(ptr+2) != '\0') {
					ptr+=2;
					skip_template_block(&ptr);
					skip_template_block(&ptr);
				}
			}
			if (*ptr != '\0') ptr++;
		}
		*tpl_cursor = ptr;
	}
}

void process_template_block(lll_buffer_t *, const char **, parameter_t[10],
	const char *, va_list, int *);

void process_conditional(lll_buffer_t *buffer, const char **tpl_cursor,
	parameter_t params[10], const char *fmt, va_list va_ptr, int *mprinted)
{
	const char *ptr = *tpl_cursor;
	unsigned int i;

	if (*ptr == '?') {
		ptr++;
		if (*ptr >= '0' && *ptr <= '9') {
			i = (*ptr) - '0';
			if (params[i].data.i != 0) {
				ptr++;
				process_template_block(buffer, &ptr,
					params, fmt, va_ptr, mprinted);
				skip_template_block(&ptr);
			} else {
				ptr++;
				skip_template_block(&ptr);
				process_template_block(buffer, &ptr,
					params, fmt, va_ptr, mprinted);
			}
			*tpl_cursor = ptr;
		}
	}
}

void process_char(lll_buffer_t *buffer, const char **tpl_cursor,
	parameter_t params[10], const char *fmt, va_list va_ptr, int *mprinted)
{
	const char *ptr = *tpl_cursor;

	switch (*ptr) {
		case '%':
			process_special(buffer, &ptr, fmt, va_ptr,
				mprinted);
			break;
		case '$':
			process_parameter(buffer, &ptr, params);
			break;
		case '?':
			process_conditional(buffer, &ptr, params, fmt,
				va_ptr, mprinted);
			break;
	}
	/* If cursor hasn't been moved then the character under the cursor has
	 * to be printed before continuing */
	if (ptr == *tpl_cursor) {
		if (*ptr == '\\') ptr++;
		bputc(*ptr, buffer);
	}

	*tpl_cursor = ptr;
}

void process_template_block(lll_buffer_t *buffer, const char **tpl_cursor,
	parameter_t params[10], const char *fmt, va_list va_ptr, int *mprinted)
{
	const char *ptr = *tpl_cursor;
	char delim = *ptr;

	ptr++;
	while (*ptr != '\0' && !(*ptr == delim && *(ptr-1) != '\\')) {
		process_char(buffer, &ptr, params, fmt, va_ptr,
			mprinted);
		if (*ptr != '\0') ptr++;
	}
	*tpl_cursor = ptr;
}

void process_template(lll_buffer_t *buffer, const char **tpl_cursor,
	parameter_t params[10], const char *fmt, va_list va_ptr, int *mprinted)
{
	const char *ptr = *tpl_cursor;

	while (*ptr != '\0') {
		process_char(buffer, &ptr, params, fmt, va_ptr,
			mprinted);
		if (*ptr != '\0') ptr++;
	}
	*tpl_cursor = ptr;
}

/* Public functions */

void lll_vfprint(FILE *stream, const char *template, va_list va_ptr)
{
	parameter_t params[10];
	const char *fmt;
	int mprinted = 0;
	lll_buffer_t buffer;

	build_params(params, va_ptr);

	fmt = va_arg(va_ptr, const char *);
	if (fmt == NULL) fmt = va_arg(va_ptr, const char *);

	lll_buffer_init(&buffer);

	process_template(&buffer, &template, params, fmt, va_ptr, &mprinted);

	if (!mprinted) {
		bputc(' ', &buffer);
		vbprintf(&buffer, fmt, va_ptr);
	}
	bprintf(&buffer, "\n");

	fputs(buffer.buf, stream);
	lll_buffer_free(&buffer);
}

void lll_vprint(const char *template, va_list va_ptr)
{
	lll_vfprint(stdout, template, va_ptr);
}

void lll_fprint(FILE *stream, const char *template, ...)
{
	va_list va_ptr;

	va_start(va_ptr, template);
	lll_vfprint(stream, template, va_ptr);
	va_end(va_ptr);
}

void lll_print(const char *template, ...)
{
	va_list va_ptr;

	va_start(va_ptr, template);
	lll_vprint(template, va_ptr);
	va_end(va_ptr);
}

void lll_vpprint(const char *path, const char *template, va_list va_ptr)
{
	FILE *stream;

	stream = fopen(path, "a");
	if (stream != NULL) {
		lll_vfprint(stream, template, va_ptr);
		fclose(stream);
	} else {
		fprintf(stderr, "Failed to open %s in append mode", path);
		perror(" ");
	}
}

void lll_pprint(const char *path, const char *template, ...)
{
	va_list va_ptr;

	va_start(va_ptr, template);
	lll_vpprint(path, template, va_ptr);
	va_end(va_ptr);
}

