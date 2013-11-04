#include <stddef.h>
#include <ruminate.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void handler( int ign ) {
	(void) ign;
	fprintf(stderr, "Hello World!\n");
}

int main( int argc, char *argv[] ) {
	signal(SIGTERM, handler);

	ruminate_init(&argc, argv, NULL);

	kill(getpid(), SIGTERM);

	fprintf(stderr, "Exiting\n");
	return 0;
}
