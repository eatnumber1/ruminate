#include <ruminate.h>

#include <glib.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

void die_if_error( GError *err ) {
	if( err == NULL ) return;

	fprintf(stderr, "%s\n", err->message);

	exit(EXIT_FAILURE);
}

int main() {
	GError *err = NULL;

	ruminate_initialize();

	Rumination *rum = rumination_new(&err);
	die_if_error(err);
	rumination_delete(&rum);

	ruminate_terminate();
}
