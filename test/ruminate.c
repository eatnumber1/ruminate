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

int main( int argc, char *argv[] ) {
#if 0
	if( argc != 2 ) {
		fprintf(stderr, "Usage: %s type\n", argv[0]);
		exit(EXIT_FAILURE);
	}
#endif

	GError *err = NULL;
	RDebugger *ruminate = r_debugger_new(&err);

#if 0
	GError *err = NULL;
	Rumination *rum = rumination_new(argv[0], &err);
	die_if_error(err);
	Type *type = rumination_find_type(rum, argv[1], &err);
	die_if_error(err);

	if( type != NULL ) {
		printf("%s\n", type_name(type));

		type_delete(&type);
	} else {
		printf("Type '%s' not found\n", argv[1]);
	}

	rumination_get_type(rum, argv, &err);

	rumination_delete(&rum);
#endif
	r_debugger_delete(ruminate);
}
