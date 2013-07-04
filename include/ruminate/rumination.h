#ifndef _RUMINATE_RUMINATION_H_
#define _RUMINATE_RUMINATION_H_

#include "ruminate/type.h"

#include <glib.h>

struct Rumination;
typedef struct Rumination Rumination;

G_BEGIN_DECLS

void rumination_delete( Rumination **rum );
Rumination *rumination_new( RDebugger *debugger, const char *exename, GError **err );

Type *rumination_get_type_by_local_variable( Rumination *rum, const char *varname, GError **err );

G_END_DECLS

#define rumination_get_type(rum, expr, err) ({ \
	typeof(expr) _expr = (expr); \
	(void) _expr; \
	rumination_get_type_by_local_variable(rum, "_expr", err); \
})

#endif
