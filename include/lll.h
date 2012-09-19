#ifndef lll_h_included
#define lll_h_included

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void
lll_log_to_stream(
	FILE *stream,
	const char *domain,     /* Name of caller library, for example */
	const char *template,   /* To define */
	uint8_t current_level,
	uint8_t level,
	const char *level_str,
	const char *filename,
	const char *function,
	uint32_t line
);

void
lll_log_to_file(
	const char *filepath,
	const char *domain,     /* Name of caller library, for example */
	const char *template,   /* To define */
	uint8_t current_level,
	uint8_t level,
	const char *level_str,
	const char *filename,
	const char *function,
	uint32_t line
);

#define warn(msg, ...) \
	lll_log_to_stream(stderr, "malib", \
		"[<time>] <level_str>: <msg> at <file>:<line> (<function>)", \
		malib_get_log_level(), 3, "WARNING", __FILE__, __func__, __LINE__)

#ifdef __cplusplus
}
#endif

#endif /* Not lll_h_included */
