#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <lll.h>

int main()
{
	int i = 0;
	int n_children = 20;

	// Disable buffering on stdout
	setvbuf(stdout, NULL, _IONBF, 0);

	while (i < n_children) {
		if (fork() == 0) {
			lll_print("pid: %p, ppid: %P");
			return 0;
		}
		i++;
	}

	i = 0;
	while (i < n_children) {
		wait(NULL);
		i++;
	}
}
