#include <stdlib.h>
#include <stdio.h>

#include <ruminate.h>
#include <jansson.h>
#include <glib.h>

#include "json-lib.h"

static void die_if_error( GError *err ) {
	if( err == NULL ) return;
	fprintf(stderr, "%s\n", err->message);
	exit(EXIT_FAILURE);
}

static json_t *serialize_void( JsonState *js, RType *rt, void *value, void *data, GError **error ) {
	(void) js, (void) rt, (void) value, (void) data, (void) error;
	return NULL;
}

static JsonSerializer void_serializer = {
	.func = serialize_void
};

int main( int argc, char *argv[] ) {
	GError *err = NULL;

	ruminate_init(&argc, argv, &err);
	die_if_error(err);

	RType *rt = ruminate_get_type(stdout, &err);
	die_if_error(err);

	JsonState *st = json_state_new();
	json_state_add_serializer(st, g_quark_from_static_string("void"), &void_serializer);

	json_t *serialized = json_serialize(st, rt, &stdout, &err);
	die_if_error(err);

	json_dumpf(serialized, stdout, 0);
	printf("\n");

	json_decref(serialized);
	r_type_unref(rt);
	return EXIT_SUCCESS;
}
