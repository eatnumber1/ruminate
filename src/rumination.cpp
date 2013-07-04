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
#include <spawn.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <sys/wait.h>

#define die_unless(...)

#define RUMINATION_DEBUGGER_CONTROLLER_PATH "./src/dbgsvr.py"

struct Rumination {
	Ice::CommunicatorPtr communicator;
	Ruminate::DebuggerFactoryPrx factory;
	Ruminate::DebuggerPrx debugger;
	Ice::AsyncResultPtr arp;
	// TODO: Deal with bad states
	GPid child_pid;
};

G_BEGIN_DECLS

__attribute__((noinline))
void rumination_hit_breakpoint() {
	// We try really hard to not be optimized away.
	asm("");
}

static void setup_env() {
	setenv("PYTHONPATH", "ice:src:.:/usr/local/lib/python2.7/site-packages", true);
}

static bool fork_child( Rumination *rum, GError **err ) {
	size_t len = strlen(RUMINATION_DEBUGGER_CONTROLLER_PATH) + 1;
	char controller_path[len];
	memcpy(controller_path, RUMINATION_DEBUGGER_CONTROLLER_PATH, len);
	char *argv[] = {
		controller_path,
		NULL
	};
	return g_spawn_async(NULL, argv, NULL, (GSpawnFlags) 0, (GSpawnChildSetupFunc) &setup_env, NULL, &rum->child_pid, err);
}

Rumination *rumination_new( int *argc, char *argv[], GError **err ) {
	Rumination *ret = new Rumination();
	if( !fork_child(ret, err) ) return NULL;
	// Give python time to start up
	sleep(1); // TODO
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
	// TODO: Check child return code
	waitpid(r->child_pid, NULL, 0);
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
