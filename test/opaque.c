#include <ruminate.h>

#include <stddef.h>
#include <stdio.h>

struct Foo;

int main( int argc, char *argv[] ) {
	ruminate_init(&argc, argv, NULL);
	struct Foo *bar = NULL;
	RPointerType *rpt = (RPointerType *) ruminate_get_type(bar, NULL);
	RAggregateType *rat = (RAggregateType *) r_pointer_type_pointee(rpt, NULL);
	printf("%zd\n", r_aggregate_type_nmembers(rat, NULL));
	r_type_unref((RType *) rat);
	r_type_unref((RType *) rpt);
}
