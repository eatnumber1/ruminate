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
	rumination_init(&argc, argv, &err);
	die_if_error(err);

	Type *type = rumination_get_type(&die_if_error, &err);
	//Type *type = rumination_get_type(rum, 1 + 1, &err);
	die_if_error(err);

	if( type != NULL ) {
		printf("%s\n", type_name(type));

		type_delete(&type);
	} else {
		printf("Type '%s' not found\n", argv[1]);
	}
}
