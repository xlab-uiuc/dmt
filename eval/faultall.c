#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv) {
	char *str;

	// we intentionally leak memory to fault all memories available to the VM.
	for (;;) {
		str = (char *) aligned_alloc(4096, 4096);
		if (!str) continue;
		str[0] = '1';
		str[2048] = '1';
	}

	return 0;
}
