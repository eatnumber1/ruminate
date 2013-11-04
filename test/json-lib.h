#ifndef _RUMINATE_JSON_H_
#define _RUMINATE_JSON_H_

#include <glib.h>
#include <jansson.h>
#include <ruminate.h>

typedef struct JsonState {
	GData *handlers; // { GQuark -> JsonSerializerCallback * }
	gint refcnt;
} JsonState;

typedef struct JsonSerializer {
	json_t *(*func)( JsonState *, RType *, void *, void *, GError ** );
	void *data;
} JsonSerializer;

JsonState *json_state_new();
JsonState *json_state_ref( JsonState * );
void json_state_unref( JsonState * );

void json_state_add_serializer( JsonState *, GQuark, JsonSerializer * );
void json_state_remove_serializer( JsonState *, GQuark );

json_t *json_serialize( JsonState *, RType *, void *, GError ** );

#endif
