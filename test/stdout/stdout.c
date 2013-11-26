#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <ruminate.h>
#include <jansson.h>
#include <glib.h>
#include <ruminate-jansson.h>

static void die_if_error( GError *err ) {
	if( err == NULL ) return;
	fprintf(stderr, "%s\n", err->message);
	exit(EXIT_FAILURE);
}

static json_t *serialize_void( JsonSerializerArgs args, void *data, GError **error ) {
	(void) args, (void) data, (void) error;
	return NULL;
}

static JsonHook void_hook = {
	.serializer = serialize_void
};

static json_t *serialize_char_ptr( JsonSerializerArgs args, void *data, GError **error ) {
	(void) data;
	g_assert(args.value != NULL);
	char *str = *((char **) args.value);
	if( str == NULL ) return json_null();

	bool is_string = false;
	while( true ) {
		if( *str == '\0' ) {
			is_string = true;
			break;
		}
		if( !isalnum(*str) ) break;
		str++;
	}

	if( is_string ) return json_string(str);
	return args.cont(args.state, args.type, args.value, error);
}

static JsonHook char_ptr_hook = {
	.serializer = serialize_char_ptr
};

int main( int argc, char *argv[] ) {
	(void) argc;
	GError *err = NULL;

	ruminate_init(argv[0], &err);
	die_if_error(err);

	RType *rt = ruminate_get_type(stdout, &err);
	die_if_error(err);

	JsonState *st = json_state_new();
	json_state_add_hook(st, g_quark_from_static_string("void"), &void_hook);
	json_state_add_hook(st, g_quark_from_static_string("char *"), &char_ptr_hook);
	json_state_set_flags(st, JSON_FLAG_SKIP_UNKNOWN);

	printf("Hello World!");
	json_t *serialized = json_serialize(st, rt, &stdout, &err);
	die_if_error(err);

	printf("\n");
	json_dumpf(serialized, stdout, 0);
	printf("\n");

	json_decref(serialized);
	r_type_unref(rt);
	return EXIT_SUCCESS;
}
