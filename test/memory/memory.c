#include <ruminate.h>

#include <glib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void die_if_error( GError *err ) {
	if( err == NULL ) return;

	fprintf(stderr, "%s\n", err->message);

	exit(EXIT_FAILURE);
}

int main( int argc, char *argv[] ) {
	(void) argc;
	GError *err = NULL;

	ruminate_init(argv[0], &err);
	die_if_error(err);

	const char *src_str = "Hello World!";
	size_t src_str_len = strlen(src_str) + 1;
	char *str = r_mem_malloc_sized(char *, src_str_len, &err);
	die_if_error(err);
	memcpy(str, src_str, src_str_len);

	RType *str_type = r_mem_type(str);
	RString *str_type_name = r_type_name(str_type, &err);
	die_if_error(err);

	printf("(%s) \"%s\"\n", r_string_bytes(str_type_name), str);

	r_string_unref(str_type_name);
	r_type_unref(str_type);
	r_mem_unref(str);
}
