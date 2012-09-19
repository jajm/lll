#include <stdio.h>
#include "lll.h"

#define logs_with_tmpl(stream, template, ...) ({ \
	fprintf(stream, "|%s|\t\t", template); \
	lll_log_to_stream(stream, template, __VA_ARGS__); \
})

#define logf_with_tmpl(filepath, template, ...) ({ \
	fprintf(stdout, "|%s| >> %s\n", template, filepath); \
	lll_log_to_file(filepath, template, __VA_ARGS__); \
})

int main(void)
{
	printf("|template|\tlog\n");
	printf("===========================================================\n");
	printf("|(null)|\t");
	lll_log_to_stream(stdout, NULL, NULL, 0, 0, NULL, NULL, 0, "test");
	logs_with_tmpl(stdout, "", NULL, 0, 0, NULL, NULL, 0, "test");
	logs_with_tmpl(stdout, "%", NULL, 0, 0, NULL, NULL, 0, "test");
	logs_with_tmpl(stdout, "%%", NULL, 0, 0, NULL, NULL, 0, "test");
	logs_with_tmpl(stdout, "%b", NULL, 0, 0, NULL, NULL, 0, "test");
	logs_with_tmpl(stdout, "%T ", NULL, 0, 0, NULL, NULL, 0, "test");
	logs_with_tmpl(stdout, "%T{}", NULL, 0, 0, NULL, NULL, 0, "test");
	logs_with_tmpl(stdout, "%T{[%D]} <%m>", NULL, 0, 0, NULL, NULL, 0,
		"test with params %d %d %d", 1, 2, 3);
	logs_with_tmpl(stdout, "{%T{%F}}{%T{%T}}", NULL, 0, 0, NULL, NULL, 0, "test");
	logs_with_tmpl(stdout, "[%d]", NULL, 0, 0, NULL, NULL, 0, "test");
	logs_with_tmpl(stdout, "[%d]", "lll", 0, 0, NULL, NULL, 0, "test");
	logs_with_tmpl(stdout, "(%l)", NULL, 0, 0, NULL, NULL, 0, "test");
	logs_with_tmpl(stdout, "(%l)", NULL, 1, 1, NULL, NULL, 0, "test");
	logs_with_tmpl(stdout, "<%F>", NULL, 0, 0, NULL, NULL, 0, "test");
	logs_with_tmpl(stdout, "<%F>", NULL, 0, 0, "test1.c", NULL, 0, "test");
	logs_with_tmpl(stdout, "in %f ", NULL, 0, 0, NULL, NULL, 0, "test");
	logs_with_tmpl(stdout, "in %f ", NULL, 0, 0, NULL, "main()", 0, "test");
	logs_with_tmpl(stdout, "at line %L: ", NULL, 0, 0, NULL, NULL, 42, "test");
	logs_with_tmpl(stdout, "[%T{%A %d %B %Y - %H:%M}] <%d> (%l) %m at %F:%L (%f)",
		"lll", 7, 3, __FILE__, __func__, __LINE__, "test complete message");
	
	logf_with_tmpl("/tmp/lll_test1.log", "[%T{%A %d %B %Y - %H:%M}] <%d> (%l) %m at %F:%L (%f)",
		"lll", 7, 3, __FILE__, __func__, __LINE__, "test complete message");
	logf_with_tmpl("/tmp/lll_test1.log", "[%T{%A %d %B %Y - %H:%M}] <%d> (%l) %m at %F:%L (%f)",
		"lll", 7, 3, __FILE__, __func__, __LINE__, "test complete message");

	logf_with_tmpl("/tmp__/lll_test1.log", "[%T{%A %d %B %Y - %H:%M}] <%d> (%l) %m at %F:%L (%f)",
		"lll", 7, 3, __FILE__, __func__, __LINE__, "test complete message");
}
