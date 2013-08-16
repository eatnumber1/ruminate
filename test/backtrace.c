#include <ruminate.h>

#include <glib.h>

#include "util.h"

void bar( int i ) {
	if( i < 2 ) {
		bar(i + 1);
	} else {
		abort_with_backtrace("Hello World!");
	}
}

void foo() {
	bar(0);
}

int main( int argc, char *argv[] ) {
	GError *err = NULL;
	rumination_init(&argc, argv, &err);
	g_assert_no_error(err);
	foo();
}
