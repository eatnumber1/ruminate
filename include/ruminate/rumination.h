#ifndef _RUMINATE_RUMINATION_H_
#define _RUMINATE_RUMINATION_H_

#include "ruminate/type.h"

#include <glib.h>

G_BEGIN_DECLS

bool rumination_destroy( GError **err );
bool rumination_init( int *argc, char *argv[], GError **err );

void rumination_begin_get_type_by_variable_name( const char *varname, GError **err );
Type *rumination_end_get_type_by_variable_name( GError **err );

__attribute__((noinline))
void rumination_hit_breakpoint();

G_END_DECLS

#define rumination_get_type(expr, err) ({ \
	typeof(expr) _expr = (expr); \
	(void) _expr; \
	rumination_begin_get_type_by_variable_name("_expr", err); \
	rumination_hit_breakpoint(); \
	rumination_end_get_type_by_variable_name(err); \
})

#endif
