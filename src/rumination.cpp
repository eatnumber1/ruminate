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

typedef struct {
	Ice::CommunicatorPtr communicator;
	Ruminate::DebuggerFactoryPrx factory;
	Ruminate::DebuggerPrx debugger;
	Ice::AsyncResultPtr arp;
	// TODO: Deal with bad states
	GPid child_pid;
} Rumination;
static Rumination *rumination;

G_BEGIN_DECLS

__attribute__((noinline))
void rumination_hit_breakpoint() {
	// We try really hard to not be optimized away.
	asm("");
}

static void setup_env() {
	setenv("PYTHONPATH", "ice:src:.:/usr/local/lib/python2.7/site-packages", true);
}

static bool fork_child( GError **err ) {
	size_t len = strlen(RUMINATION_DEBUGGER_CONTROLLER_PATH) + 1;
	char controller_path[len];
	memcpy(controller_path, RUMINATION_DEBUGGER_CONTROLLER_PATH, len);
	char *argv[] = {
		controller_path,
		NULL
	};
	return g_spawn_async(NULL, argv, NULL, (GSpawnFlags) 0, (GSpawnChildSetupFunc) &setup_env, NULL, &rumination->child_pid, err);
}

__attribute__((destructor))
static void rumination_destroy_atexit() {
	// TODO: Error handling
	rumination_destroy(NULL);
}

bool rumination_init( int *argc, char *argv[], GError **err ) {
	// TODO Thread safety
	if( rumination != NULL ) return true;
	rumination = new Rumination();
	if( !fork_child(err) ) return false;
	// Give python time to start up
	sleep(1); // TODO
	rumination->communicator = Ice::initialize(*argc, argv);
	auto factory_proxy = rumination->communicator->stringToProxy("DebuggerFactory:default -p 1024");
	rumination->factory = Ruminate::DebuggerFactoryPrx::checkedCast(factory_proxy);
	die_unless(rumination->factory);

	Ruminate::DebuggerFactoryOptions opts;

	opts.exename = argv[0];
	static_assert(sizeof(::Ice::Long) >= sizeof(pid_t), "A pid cannot fit in a long!");
	opts.pid = getpid();
	static_assert(sizeof(::Ice::Long) >= sizeof(size_t), "A pointer cannot fit in a long!");
	opts.breakpointAddress = (::Ice::Long) &rumination_hit_breakpoint;

	rumination->debugger = rumination->factory->create(opts);
	die_unless(rumination->debugger);

	// TODO: Error handling
	atexit(rumination_destroy_atexit);

	return true;
}

bool rumination_destroy( GError **err ) {
	(void) err;
	if( rumination == NULL ) return true;
	rumination->factory->shutdown();
	rumination->communicator->destroy();
	// TODO: Check child return code
	waitpid(rumination->child_pid, NULL, 0);
	delete rumination;
	rumination = NULL;

	return true;
}


void rumination_begin_get_type_by_variable_name( const char *varname, GError **err ) {
	rumination->arp = rumination->debugger->begin_getTypeByVariableName(varname, gettid());
}

Type *rumination_end_get_type_by_variable_name( GError **err ) {
	Ruminate::TypePrx t(rumination->debugger->end_getTypeByVariableName(rumination->arp));
	rumination->arp = NULL;
	return type_new(t, err);
}

G_END_DECLS
