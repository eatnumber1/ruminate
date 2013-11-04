#include <ruminate.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef char *string_t;
struct foo { string_t str; int i; };

void print_data( RType *type, const void *data ) {
	switch( r_type_id(type, NULL) ) {
		case R_TYPE_TYPEDEF:
			if( strcmp(r_string_bytes(r_type_name(type, NULL)), "string_t") == 0 )
				printf("(string_t) \"%s\"\n", *((const string_t *) data));
			break;
		case R_TYPE_BUILTIN:
			switch( r_builtin_type_id((RBuiltinType *) type, NULL) ) {
				case R_BUILTIN_TYPE_INT:
					printf("(int) %d\n", *((const int *) data));
					break;
			}
			break;
		case R_TYPE_AGGREGATE: {
			RAggregateType *agg = (RAggregateType *) type;
			if( r_aggregate_type_id(agg, NULL) == R_AGGREGATE_TYPE_STRUCT ) {
				printf("(%s) {\n", r_string_bytes(r_type_name(type, NULL)));
				for( size_t i = 0; i < r_aggregate_type_nmembers(agg, NULL); i++ ) {
					RAggregateMember *memb = r_aggregate_type_member_at(agg, i, NULL);
					printf("\t.%s = ", r_string_bytes(r_aggregate_member_name(memb, NULL)));
					RTypeMember *tmemb = (RTypeMember *) memb;
					ptrdiff_t offset = r_type_member_offset(tmemb, NULL);
					print_data(r_type_member_type(tmemb, NULL), data + offset);
				}
				printf("}\n");
			}
			break;
		}
	}
}

int main( int argc, char *argv[] ) {
	ruminate_init(&argc, argv, NULL);
	struct foo bar = {
		.str = "Hello World!",
		.i = 6666
	};
	print_data(ruminate_get_type(bar, NULL), &bar);
}
