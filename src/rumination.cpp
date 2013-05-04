#include "ruminate/rumination.h"
#include "ruminate/errors.h"

#include <glib.h>
#include <lldb/API/LLDB.h>
#include <lldb/API/SBStream.h>

using namespace lldb;

struct Rumination {
	SBDebugger debugger;
	SBTarget target;
};

static void log_output_callback( const char *msg, void *baton ) {
	g_message("%s", msg);
}

Rumination *rumination_new( const char *exename, GError **err ) {

	Rumination *rum = g_slice_new(Rumination);
	new (rum) Rumination();

	rum->debugger = SBDebugger::Create(false, log_output_callback, NULL);
	const char *all[] = { "all", NULL };
#if 0
	rum->debugger.EnableLog("dwarf", all);
	rum->debugger.EnableLog("lldb", all);
	rum->debugger.EnableLog("kdp-remote", all);
	rum->debugger.EnableLog("gdb-remote", all);
#endif

	if( !rum->debugger.IsValid() ) {
		g_set_error(
			err,
			RUMINATE_ERROR,
			RUMINATE_ERROR_SB_INVALID,
			"SBDebugger instance invalid"
		);
		goto debugger_invalid;
	}

	rum->target = rum->debugger.CreateTarget(exename);
	if( !rum->target.IsValid() ) {
		g_set_error(
			err,
			RUMINATE_ERROR,
			RUMINATE_ERROR_SB_INVALID,
			"SBTarget instance invalid"
		);
		goto target_invalid;
	}

	return rum;

target_invalid:
	SBDebugger::Destroy(rum->debugger);
debugger_invalid:
	g_slice_free(Rumination, rum);

	return NULL;
}

void rumination_delete( Rumination **rum ) {
	g_return_if_fail(rum != NULL && *rum != NULL);

	Rumination *r = *rum;
	SBDebugger::Destroy(r->debugger);
	r->~Rumination();
	g_slice_free(Rumination, r);
	*rum = NULL;
}

void rumination_gettype( Rumination *rum, const char *type_name, GError **err ) {
	SBStream out;
	out.RedirectToFileHandle(stdout, false);

	for( uint32_t i = 0; i < rum->target.GetNumModules(); i++ ) {
		SBModule module(rum->target.GetModuleAtIndex(i));
		if( !module.IsValid() ) continue;

		SBTypeList type_list(module.FindTypes(type_name));
		if( !type_list.IsValid() ) continue;

		for( uint32_t x = 0; x < type_list.GetSize(); i++ ) {
			SBType type(type_list.GetTypeAtIndex(i));
			if( !type.IsValid() ) continue;

			type.GetDescription(out, eDescriptionLevelFull);
			out.Printf("\n");
		}
	}
}
