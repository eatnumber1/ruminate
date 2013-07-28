#include <stdint.h>

#include <glib.h>

#include <Ice/Ice.h>
#include "ice/frame.h"

#include "ruminate/type.h"
#include "ruminate/frame.h"

#include "private/type.h"
#include "private/frame.h"

struct RFrameList {
	RFrame **frames;
	size_t nframes;
	gint refcnt;
};

struct RFrame {
	RType *functionType;
	Ruminate::Frame frame;
	gint refcnt;
};

G_BEGIN_DECLS

size_t r_frame_list_size( RFrameList *list ) {
	return list->nframes;
}

RFrame *r_frame_list_at( RFrameList *list, size_t i ) {
	return list->frames[i];
}

RFrameList *r_frame_list_new( Ruminate::FrameList list, GError **err ) {
	RFrameList *ret = g_slice_new(RFrameList);
	new (ret) RFrameList();
	ret->refcnt = 1;
	ret->nframes = list.size();
	ret->frames = g_new(RFrame *, ret->nframes);
	size_t ncompleted = 0;
	for( size_t i = 0; i < ret->nframes; i++ ) {
		if( (ret->frames[i] = r_frame_new(list[i], err)) == NULL )
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

void r_frame_list_ref( RFrameList *list ) {
	g_atomic_int_inc(&list->refcnt);
}

void r_frame_list_unref( RFrameList *list ) {
	if( g_atomic_int_dec_and_test(&list->refcnt) ) {
		list->~RFrameList();
		for( size_t i = 0; i < list->nframes; i++ )
			r_frame_unref(list->frames[i]);
		g_free(list->frames);
		g_slice_free(RFrameList, list);
	}
}

RFrame *r_frame_new( Ruminate::Frame frame, GError **err ) {
	RFrame *ret = g_slice_new(RFrame);
	new (ret) RFrame();
	ret->refcnt = 1;
	ret->frame = frame;
	return ret;
}

void r_frame_ref( RFrame *frame ) {
	g_atomic_int_inc(&frame->refcnt);
}

void r_frame_unref( RFrame *frame ) {
	if( g_atomic_int_dec_and_test(&frame->refcnt) ) {
		frame->~RFrame();
		g_slice_free(RFrame, frame);
	}
}

const char *r_frame_function_name( RFrame *rf ) {
	return rf->frame.functionName.c_str();
}

const char *r_frame_module_name( RFrame *rf ) {
	return rf->frame.moduleName.c_str();
}

const char *r_frame_compile_unit_name( RFrame *rf ) {
	return rf->frame.compileUnitName.c_str();
}

RType *r_frame_function_type( RFrame *rf, GError **err ) {
	if( rf->functionType != NULL ) {
		r_type_ref(rf->functionType);
		return rf->functionType;
	}
	rf->functionType = r_type_new(rf->frame.functionType, err);
	return rf->functionType;
}

uint32_t r_frame_line( RFrame *rf ) {
	return rf->frame.line;
}

G_END_DECLS
