#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include <glib.h>

#include "ruminate.h"

#include "util.h"

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
	size_t frames_len = r_frame_list_size(frames, &err);
	abort_if_error(err);
	for( size_t i = 0; i < frames_len; i++ ) {
		RFrame *frame = r_frame_list_at(frames, i, &err);
		abort_if_error(err);
		RString *fname = r_frame_function_name(frame, &err);
		abort_if_error(err);
		RString *mname = r_frame_module_name(frame, &err);
		abort_if_error(err);
		RString *cuname = r_frame_compile_unit_name(frame, &err);
		abort_if_error(err);
		uint32_t line = r_frame_line(frame, &err);
		abort_if_error(err);
		fprintf(
			stderr,
			"\tat %s(%s, %s:%d)\n",
			r_string_bytes(fname),
			r_string_bytes(mname),
			r_string_bytes(cuname),
			line
		);
		r_string_unref(cuname);
		r_string_unref(mname);
		r_string_unref(fname);
	}

	r_frame_list_unref(frames);
	abort();
}
