#include <ruminate.h>

#include <stddef.h>
#include <stdio.h>

int main( int argc, char *argv[] ) {
	ruminate_init(&argc, argv, NULL);
	int foo[argc];
	RPointerType *rpt = (RPointerType *) ruminate_get_type(&foo, NULL);
	RArrayType *rat = (RArrayType *) r_pointer_type_pointee(rpt, NULL);
	printf("%zd\n", r_array_type_size(rat, NULL));
	r_type_unref((RType *) rat);
	r_type_unref((RType *) rpt);
}
