#include <stdint.h>
#include <stdio.h>
#include "lll.h"

#define logs(stream, template, ...) ({ \
	fprintf(stream, "lll_fprint(" #stream ", \"%s\", ", template); \
	fputs(#__VA_ARGS__, stream); \
	fputs(");", stream); \
	fprintf(stream, "\n>>> "); \
	lll_fprint(stream, template, __VA_ARGS__); \
	fprintf(stream, "\n"); \
})

#define logf(filepath, template, ...) ({ \
	printf("lll_pprint(\"%s\", \"%s\", ", filepath, template); \
	fputs(#__VA_ARGS__, stdout); \
	puts(");"); \
	lll_pprint(filepath, template, __VA_ARGS__); \
})

int main(void)
{
	logs(stdout, "", NULL, NULL, "message");
	logs(stdout, "", NULL, NULL, "message with param %d %s", 2, "deux");
	logs(stdout, "%m", NULL, NULL, "message");
	logs(stdout, "[%m]", NULL, NULL, "message");
	logs(stdout, "[%T]", NULL, NULL, "message");
	logs(stdout, "[%T{}]", NULL, NULL, "message");
	logs(stdout, "[%T{%H:%M}]", NULL, NULL, "message");
	logs(stdout, "[%T{%H:%m]", NULL, NULL, "message");
	logs(stdout, "[pid:%p ppid:%P]", NULL, NULL, "message");
	logs(stdout, "$0", "%s", "test string param", NULL, NULL, "message");
	logs(stdout, "$1", "%s", "test string param", NULL, NULL, "message");
	logs(stdout, "?0:-$0-:empty:", "%s", "test string param", NULL, NULL, "message");
	logs(stdout, "?0:-$0-:empty:", NULL, NULL, "message");
	logs(stdout, "?0:-$0-::", "%s", "test string param", NULL, NULL, "message");
	logs(stdout, "?0:-$0-::", NULL, NULL, "message");
	logs(stdout, "?0:-$0-:oops...  ", NULL, NULL, "message");
	logs(stdout, "?0:param\\:$0:: end of", "%s", "test string param", NULL, NULL, "message");
	logs(stdout, "?0:at $0?1:\\:$1:::no file:", "%s", __FILE__, "%d", __LINE__, NULL, NULL, "message");
	logs(stdout, "?0:at $0?1:\\:$1:::no file:", "%s", __FILE__, "%d", 0, NULL, NULL, "message");
	logs(stdout, "?0:at $0?1:\\:$1:::no file:", "%s", NULL, "%d", __LINE__, NULL, NULL, "message");
	logs(stdout, "?0:at $0?1:\\:$1:::no file:", "%s", NULL, "%d", 0, NULL, NULL, "message");
	logs(stdout, "?0:at $0?1|:$1||:no file:", "%s", __FILE__, "%d", __LINE__, NULL, NULL, "message");
	logs(stdout, "?0:at $0?1|:$1||:no file:", "%s", __FILE__, "%d", 0, NULL, NULL, "message");
	logs(stdout, "?0:at $0?1|:$1||:no file:", "%s", NULL, "%d", __LINE__, NULL, NULL, "message");
	logs(stdout, "?0:at $0?1|:$1||:no file:", "%s", NULL, "%d", 0, NULL, NULL, "message");
	logs(stdout, "$a", "%d", 2, NULL, NULL, "message");
	logs(stdout, "double $0", "%e", 123.456, NULL, NULL, "message");
	logs(stdout, "long double $0", "%Le", 123.456L, NULL, NULL, "message");
	logf("/tmp/test.log", "%m at $0", "%s", __FILE__, NULL, NULL, "message");

	return 0;
}
