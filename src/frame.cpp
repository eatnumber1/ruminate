#include <functional>
#include <memory>
#include <string>
#include <cstddef>
#include <cstdint>
#include <utility>

#include <Ice/Ice.h>
#include "ice/frame.h"

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/type.h"
#include "ruminate/frame.h"

#define _FRAME_CPP_

#include "private/common.h"
#include "private/type.h"
#include "private/frame.h"

RFrame *r_frame_new( Ruminate::Frame &&frame, GError ** ) noexcept {
	RFrame *ret = g_slice_new(RFrame);
	ret->refcnt = 1;
	ret->frame = frame;
	return ret;
}

RFrameList *r_frame_list_new( Ruminate::FrameList &&list, GError **error ) noexcept {
	RFrameList *ret = g_slice_new(RFrameList);
	ret->refcnt = 1;
	ret->nframes = list.size();
	ret->frames = g_new(RFrame *, ret->nframes);
	size_t ncompleted = 0;
	for( size_t i = 0; i < ret->nframes; i++ ) {
		if( (ret->frames[i] = r_frame_new(std::move(list[i]), error)) == NULL )
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

size_t r_frame_list_size( RFrameList *list, GError ** ) noexcept {
	return list->nframes;
}

RFrame *r_frame_list_at( RFrameList *list, size_t i, GError ** ) noexcept {
	return list->frames[i];
}

void r_frame_list_ref( RFrameList *list ) noexcept {
	g_atomic_int_inc(&list->refcnt);
}

void r_frame_list_unref( RFrameList *list ) noexcept {
	if( g_atomic_int_dec_and_test(&list->refcnt) ) {
		for( size_t i = 0; i < list->nframes; i++ )
			r_frame_unref(list->frames[i]);
		g_free(list->frames);
		g_slice_free(RFrameList, list);
	}
}

void r_frame_ref( RFrame *frame ) noexcept {
	g_atomic_int_inc(&frame->refcnt);
}

void r_frame_unref( RFrame *frame ) noexcept {
	if( g_atomic_int_dec_and_test(&frame->refcnt) )
		g_slice_free(RFrame, frame);
}

const char *r_frame_function_name( RFrame *rf, GError ** ) noexcept {
	return rf->frame.functionName.c_str();
}

const char *r_frame_module_name( RFrame *rf, GError ** ) noexcept {
	return rf->frame.moduleName.c_str();
}

const char *r_frame_compile_unit_name( RFrame *rf, GError ** ) noexcept {
	return rf->frame.compileUnitName.c_str();
}

RType *r_frame_function_type( RFrame *rf, GError **err ) noexcept {
	if( rf->functionType != NULL ) return r_type_ref(rf->functionType);
	rf->functionType = r_type_new(rf->frame.functionType, err);
	return rf->functionType;
}

uint32_t r_frame_line( RFrame *rf, GError ** ) noexcept {
	return rf->frame.line;
}

G_END_DECLS
