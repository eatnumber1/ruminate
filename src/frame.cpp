#include <exception>
#include <sstream>
#include <cstddef>
#include <new>
#include <utility>

#include <stdint.h>

#include <Ice/Ice.h>
#include "ice/frame.h"

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/type.h"
#include "ruminate/frame.h"

#include "private/common.h"
#include "private/string.h"
#include "private/memory.h"
#include "private/type.h"
#include "private/frame.h"

RFrame *r_frame_new( Ruminate::Frame &frame, GError ** ) RUMINATE_NOEXCEPT {
	RFrame *ret = g_slice_new(RFrame);
	new (ret) RFrame();
	ret->refcnt = 1;
	ret->frame = frame;
	ret->function_name = NULL;
	ret->module_name = NULL;
	ret->compile_unit_name = NULL;
	return ret;
}

RFrameList *r_frame_list_new( Ruminate::FrameList &list, GError **error ) RUMINATE_NOEXCEPT {
	RFrameList *ret = g_slice_new(RFrameList);
	new (ret) RFrameList();
	ret->refcnt = 1;
	ret->nframes = list.size();
	ret->frames = g_new(RFrame *, ret->nframes);
	size_t ncompleted = 0;
	for( size_t i = 0; i < ret->nframes; i++ ) {
		if( (ret->frames[i] = r_frame_new(list[i], error)) == NULL )
			goto out_r_frame_new;
		ncompleted++;
	}
	return ret;

out_r_frame_new:
	for( size_t i = 0; i < ncompleted; i++ )
		r_frame_unref(ret->frames[i]);
	g_free(ret->frames);
	g_slice_free(RFrameList, ret);
	return NULL;
}

G_BEGIN_DECLS

size_t r_frame_list_size( RFrameList *list, GError ** ) RUMINATE_NOEXCEPT {
	return list->nframes;
}

RFrame *r_frame_list_at( RFrameList *list, size_t i, GError ** ) RUMINATE_NOEXCEPT {
	return list->frames[i];
}

void r_frame_list_ref( RFrameList *list ) RUMINATE_NOEXCEPT {
	g_atomic_int_inc(&list->refcnt);
}

void r_frame_list_unref( RFrameList *list ) RUMINATE_NOEXCEPT {
	if( g_atomic_int_dec_and_test(&list->refcnt) ) {
		for( size_t i = 0; i < list->nframes; i++ )
			r_frame_unref(list->frames[i]);
		g_free(list->frames);
		list->~RFrameList();
		g_slice_free(RFrameList, list);
	}
}

void r_frame_ref( RFrame *frame ) RUMINATE_NOEXCEPT {
	g_atomic_int_inc(&frame->refcnt);
}

void r_frame_unref( RFrame *frame ) RUMINATE_NOEXCEPT {
	if( g_atomic_int_dec_and_test(&frame->refcnt) ) {
		if( frame->function_name != NULL ) r_string_unref(frame->function_name);
		if( frame->module_name != NULL ) r_string_unref(frame->module_name);
		if( frame->compile_unit_name != NULL ) r_string_unref(frame->compile_unit_name);
		frame->~RFrame();
		g_slice_free(RFrame, frame);
	}
}

RString *r_frame_function_name( RFrame *rf, GError ** ) RUMINATE_NOEXCEPT {
	if( rf->function_name == NULL )
		rf->function_name = r_string_new(rf->frame.functionName);

	return r_string_ref(rf->function_name);
}

RString *r_frame_module_name( RFrame *rf, GError ** ) RUMINATE_NOEXCEPT {
	if( rf->module_name == NULL )
		rf->module_name = r_string_new(rf->frame.moduleName);

	return r_string_ref(rf->module_name);
}

RString *r_frame_compile_unit_name( RFrame *rf, GError ** ) RUMINATE_NOEXCEPT {
	if( rf->compile_unit_name == NULL )
		rf->compile_unit_name = r_string_new(rf->frame.compileUnitName);

	return r_string_ref(rf->compile_unit_name);
}

RType *r_frame_function_type( RFrame *rf, GError **err ) RUMINATE_NOEXCEPT {
	if( rf->functionType != NULL ) return r_type_ref(rf->functionType);
	// TODO: Maybe retrieve the fp from the SBValue?
	rf->functionType = r_type_new(rf->frame.functionType, NULL, NULL, err);
	return rf->functionType;
}

uint32_t r_frame_line( RFrame *rf, GError ** ) RUMINATE_NOEXCEPT {
	return rf->frame.line;
}

G_END_DECLS
