#ifndef _RUMINATE_INIT_H_
#define _RUMINATE_INIT_H_

#include <glib.h>

struct _RDebugger;
typedef struct _RDebugger RDebugger;

G_BEGIN_DECLS

RDebugger *r_debugger_new( GError **err );
void r_debugger_delete( RDebugger *rum );

G_END_DECLS

#endif
