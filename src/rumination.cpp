#include "private/gettid.h"
#include "private/type.h"

#include "ruminate/rumination.h"
#include "ruminate/errors.h"

#include <Ice/Ice.h>
#include "ice/debugger_factory.h"
#include "ice/debugger.h"
#include "ice/type.h"

#include <glib.h>

#include <unistd.h>

#define die_unless(...)

struct Rumination {
	Ice::CommunicatorPtr communicator;
	Ruminate::DebuggerFactoryPrx factory;
	Ruminate::DebuggerPrx debugger;
	Ice::AsyncResultPtr arp;
	// TODO: Deal with bad states
};

G_BEGIN_DECLS

__attribute__((noinline))
void rumination_hit_breakpoint() {
	// We try really hard to not be optimized away.
	asm("");
}

Rumination *rumination_new( int *argc, char *argv[], GError **err ) {
	Rumination *ret = new Rumination();
	ret->communicator = Ice::initialize(*argc, argv);
	auto factory_proxy = ret->communicator->stringToProxy("DebuggerFactory:default -p 1024");
	ret->factory = Ruminate::DebuggerFactoryPrx::checkedCast(factory_proxy);
	die_unless(ret->factory);

	Ruminate::DebuggerFactoryOptions opts;

	opts.exename = argv[0];
	static_assert(sizeof(::Ice::Long) >= sizeof(pid_t), "A pid cannot fit in a long!");
	opts.pid = getpid();
	static_assert(sizeof(::Ice::Long) >= sizeof(size_t), "A pointer cannot fit in a long!");
	opts.breakpointAddress = (::Ice::Long) &rumination_hit_breakpoint;

	ret->debugger = ret->factory->create(opts);
	die_unless(ret->debugger);

	return ret;
}

void rumination_delete( Rumination **rum ) {
	if( rum == NULL ) return;
	Rumination *r = *rum;
	r->factory->shutdown();
	r->communicator->destroy();
	delete r;
	*rum = NULL;
}


void rumination_begin_get_type_by_variable_name( Rumination *rum, const char *varname, GError **err ) {
	rum->arp = rum->debugger->begin_getTypeByVariableName(varname, gettid());
}

Type *rumination_end_get_type_by_variable_name( Rumination *rum, GError **err ) {
	Ruminate::TypePrx t(rum->debugger->end_getTypeByVariableName(rum->arp));
	rum->arp = NULL;
	return type_new(t, err);
}

G_END_DECLS
