#include "private/lldb/sbdebugger.h"

#include "ruminate/rumination.h"

#include <glib.h>
#include <stddef.h>

struct Rumination {
	SBDebugger *debugger;
};

void rumination_delete( Rumination **rum ) {
	g_return_if_fail(rum != NULL && *rum != NULL);

	sbdebugger_destroy((*rum)->debugger);
	g_slice_free(Rumination, *rum);
	*rum = NULL;
}

Rumination *rumination_new( GError **err ) {
	Rumination *ret = g_slice_new(Rumination);
	ret->debugger = sbdebugger_create(err);
	return ret;

sbdebugger_fail:
	g_slice_free(Rumination, ret);

	return NULL;
}
