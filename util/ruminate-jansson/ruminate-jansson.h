#ifndef _RUMINATE_JANSSON_H_
#define _RUMINATE_JANSSON_H_

#include <glib.h>
#include <jansson.h>
#include <ruminate.h>

typedef enum JsonFlags {
	JSON_FLAG_BIJECTIVE = 1 << 0
} JsonFlags;

typedef struct JsonState JsonState;

typedef json_t *(*JsonSerializer)( JsonState *, RType *, void *, void *, GError ** );
typedef void *(*JsonDeserializer)( JsonState *, RType *, json_t *, void *, GError ** );

typedef struct JsonHook {
	JsonSerializer serializer;
	JsonDeserializer deserializer;
	void *serializer_data, *deserializer_data;
} JsonHook;

JsonState * RUMINATE_EXPORT json_state_new(void);
JsonState * RUMINATE_EXPORT json_state_ref( JsonState * );
void RUMINATE_EXPORT json_state_unref( JsonState * );

void RUMINATE_EXPORT json_state_add_hook( JsonState *, GQuark, JsonHook * );
void RUMINATE_EXPORT json_state_remove_hook( JsonState *, GQuark );

void RUMINATE_EXPORT json_state_set_flags( JsonState *, JsonFlags );
JsonFlags RUMINATE_EXPORT json_state_get_flags( JsonState * );

json_t * RUMINATE_EXPORT json_serialize( JsonState *, RType *, void *, GError ** );
void * RUMINATE_EXPORT json_deserialize( JsonState *, json_t *, GError ** );

#endif
