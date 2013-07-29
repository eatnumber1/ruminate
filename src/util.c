#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include <glib.h>

#include "ruminate/util.h"
#include "ruminate/type.h"
#include "ruminate/frame.h"
#include "ruminate/rumination.h"

void abort_if_error( GError *err ) {
	if( err == NULL ) return;

	fprintf(stderr, "%s\n", err->message);
	abort();
}

void abort_with_backtrace( const char *message ) {
	GError *err = NULL;

	RFrameList *frames = rumination_backtrace(&err);
	abort_if_error(err);

	fprintf(stderr, "abort(): %s\n", message == NULL ? "" : message);
	for( size_t i = 0; i < r_frame_list_size(frames); i++ ) {
		RFrame *frame = r_frame_list_at(frames, i);
		const char *fname = r_frame_function_name(frame);
		const char *mname = r_frame_module_name(frame);
		const char *cuname = r_frame_compile_unit_name(frame);
		uint32_t line = r_frame_line(frame);
		fprintf(stderr, "\tat %s(%s, %s:%d)\n", fname, mname, cuname, line);
	}

	r_frame_list_unref(frames);
	abort();
}
