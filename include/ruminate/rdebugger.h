#ifndef _RUMINATE_RDEBUGGER_H_
#define _RUMINATE_RDEBUGGER_H_

#include <glib.h>

struct _RDebugger;
typedef struct _RDebugger RDebugger;

G_BEGIN_DECLS

RDebugger *r_debugger_new( GError **err );
void r_debugger_delete( RDebugger *rum );

void r_debugger_get_type_by_variable_name( const char *name );

G_END_DECLS

#endif
