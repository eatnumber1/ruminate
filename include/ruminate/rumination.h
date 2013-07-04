#ifndef _RUMINATE_RUMINATION_H_
#define _RUMINATE_RUMINATION_H_

#include "ruminate/type.h"

#include <glib.h>

struct Rumination;
typedef struct Rumination Rumination;

G_BEGIN_DECLS

void rumination_delete( Rumination **rum );
Rumination *rumination_new( int *argc, char *argv[], GError **err );

void rumination_begin_get_type_by_variable_name( Rumination *rum, const char *varname, GError **err );
Type *rumination_end_get_type_by_variable_name( Rumination *rum, GError **err );

__attribute__((noinline))
void rumination_hit_breakpoint();

G_END_DECLS

#define rumination_get_type(rum, expr, err) ({ \
	typeof(expr) _expr = (expr); \
	(void) _expr; \
	rumination_begin_get_type_by_variable_name(rum, "_expr", err); \
	rumination_hit_breakpoint(); \
	rumination_end_get_type_by_variable_name(rum, err); \
})

#endif
