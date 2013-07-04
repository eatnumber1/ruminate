#include "private/gettid.h"
#include "private/rdebugger.h"
#include "private/type.h"

#include "ruminate/rumination.h"
#include "ruminate/errors.h"

#include <Ice/Ice.h>
#include "ice/debugger.h"
#include "ice/type.h"

#include <glib.h>

#include <unistd.h>

#define die_unless(...)

struct Rumination {
	Ruminate::DebuggerPrx debugger;
};

G_BEGIN_DECLS

__attribute__((noinline))
static void gettype_breakpoint_fn() {
	// We try really hard to not be optimized away.
	asm("");
}

Rumination *rumination_new( RDebugger *debugger, const char *exename, GError **err ) {
	Rumination *ret = new Rumination();
	Ruminate::DebuggerFactoryOptions opts;

	opts.exename = exename;
	static_assert(sizeof(::Ice::Long) >= sizeof(pid_t), "A pid cannot fit in a long!");
	opts.pid = getpid();
	static_assert(sizeof(::Ice::Long) >= sizeof(size_t), "A pointer cannot fit in a long!");
	opts.breakpointAddress = (::Ice::Long) &gettype_breakpoint_fn;

	ret->debugger = rdebugger_get_factory(debugger)->create(opts);
	die_unless(ret->debugger);

	return ret;
}

void rumination_delete( Rumination **rum ) {
	if( rum == NULL ) return;
	delete *rum;
	*rum = NULL;
}

Type *rumination_get_type_by_local_variable( Rumination *rum, const char *varname, GError **err ) {
	Ice::AsyncResultPtr arp = rum->debugger->begin_getTypeByVariableName(varname, gettid());
	gettype_breakpoint_fn();
	Ruminate::TypePrx t = rum->debugger->end_getTypeByVariableName(arp);
	return type_new(t, err);
}

G_END_DECLS
