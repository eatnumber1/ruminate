#include "private/lldb/sbdebugger.h"

#include "ruminate/errors.h"

#include <lldb/API/SBDebugger.h>

#include <exception>

#include <glib.h>

using namespace lldb;

void sbdebugger_initialize() {
	SBDebugger::Initialize();
}

void sbdebugger_terminate() {
	SBDebugger::Terminate();
}

SBDebugger *sbdebugger_create( GError **err ) {
	SBDebugger dbg = SBDebugger::Create(false);
	if( !dbg.IsValid() ) goto sb_invalid_create;

	SBDebugger *ret;

	try {
		ret = new SBDebugger(dbg);
	} catch( std::exception &ex ) {
		SBDebugger::Destroy(dbg);
		g_set_error_literal(
			err,
			RUMINATE_ERROR,
			RUMINATE_ERROR_STD_EXCEPTION,
			ex.what()
		);
		return NULL;
	}
	if( !ret->IsValid() ) goto sb_invalid_copy;

	return ret;

sb_invalid_copy:
	delete ret;
sb_invalid_create:
	SBDebugger::Destroy(dbg);

	g_set_error(
		err,
		RUMINATE_ERROR,
		RUMINATE_ERROR_SB_INVALID,
		"SBDebugger instance invalid"
	);

	return NULL;
}

void sbdebugger_destroy( SBDebugger *debugger ) {
	SBDebugger::Destroy(*debugger);
	delete debugger;
}

bool sbdebugger_is_valid( SBDebugger *debugger ) {
	return debugger->IsValid();
}
