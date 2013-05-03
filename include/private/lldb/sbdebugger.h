#ifndef _PRIVATE_LLDB_SBDEBUGGER_H_
#define _PRIVATE_LLDB_SBDEBUGGER_H_

#include <glib.h>

#include <stdbool.h>

#ifdef __cplusplus
#include <lldb/API/SBDebugger.h>
typedef lldb::SBDebugger SBDebugger;
#else
typedef void SBDebugger;
#endif

G_BEGIN_DECLS

void sbdebugger_initialize();
void sbdebugger_terminate();

SBDebugger *sbdebugger_create( GError **err );
void sbdebugger_destroy( SBDebugger *debugger );

bool sbdebugger_is_valid( SBDebugger *debugger );

G_END_DECLS

#endif
