#ifndef _RUMINATION_H_
#define _RUMINATION_H_

#include <glib.h>

struct Rumination;
typedef struct Rumination Rumination;

G_BEGIN_DECLS

void rumination_delete( Rumination **rum );
Rumination *rumination_new( const char *exename, GError **err );

void rumination_gettype( Rumination *rum, const char *type_name, GError **err );

G_END_DECLS

#endif
