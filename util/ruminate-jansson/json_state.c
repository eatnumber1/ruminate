#include <stddef.h>
#include <stdbool.h>

#include <glib.h>
#include <jansson.h>
#include <ruminate.h>

#include "ruminate-jansson.h"
#include "common.h"

void json_state_set_flags( JsonState *js, JsonFlags jf ) {
	js->flags = jf;
}

JsonFlags json_state_get_flags( JsonState *js ) {
	return js->flags;
}

JsonState *json_state_new() {
	JsonState *ret = g_slice_new(JsonState);
	ret->refcnt = 1;
	ret->flags = 0;
	g_datalist_init(&ret->handlers);
	return ret;
}

JsonState *json_state_ref( JsonState *js ) {
	g_atomic_int_inc(&js->refcnt);
	return js;
}

void json_state_unref( JsonState *js ) {
	if( g_atomic_int_dec_and_test(&js->refcnt) ) {
		g_datalist_clear(&js->handlers);
		g_slice_free(JsonState, js);
	}
}

void json_state_add_hook( JsonState *js, GQuark id, JsonHook *ser ) {
	g_datalist_id_set_data(&js->handlers, id, ser);
}

void json_state_remove_hook( JsonState *js, GQuark id ) {
	g_datalist_id_remove_data(&js->handlers, id);
}
