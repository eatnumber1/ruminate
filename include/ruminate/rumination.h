#ifndef _RUMINATE_RUMINATION_H_
#define _RUMINATE_RUMINATION_H_

#include "ruminate/type.h"

#include <glib.h>

struct Rumination;
typedef struct Rumination Rumination;

G_BEGIN_DECLS

void rumination_delete( Rumination **rum );
Rumination *rumination_new( const char *exename, GError **err );

Type *rumination_find_type( Rumination *rum, const char *type_name, GError **err );
Type *rumination_get_type( Rumination *rum, void *obj, GError **err );

G_END_DECLS

#endif
