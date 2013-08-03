#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <spawn.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdbool.h>

#include <glib.h>

#include <Ice/Ice.h>
#include "ice/debugger_factory.h"
#include "ice/debugger.h"
#include "ice/frame.h"
#include "ice/type.h"

#define RUMINATE_PRIVATE

#include "ruminate/type.h"
#include "ruminate/frame.h"
#include "ruminate/rumination.h"
#include "ruminate/errors.h"

#include "private/gettid.h"
#include "private/type.h"
#include "private/frame.h"

#define die_unless(...)

#define RUMINATION_DEBUGGER_CONTROLLER_PATH "./src/dbgsvr.py"

typedef struct {
	Ice::CommunicatorPtr communicator;
	Ruminate::DebuggerFactoryPrx factory;
	Ruminate::DebuggerPrx debugger;
	// TODO: Deal with bad states
	Ice::AsyncResultPtr arp;
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
	// TODO: Remove this
	setenv("PYTHONPATH", "ice:src:.:/usr/local/lib/python2.7/site-packages", true);
}

static gint fork_child( GError **err ) {
	size_t len = strlen(RUMINATION_DEBUGGER_CONTROLLER_PATH) + 1;
	char controller_path[len];
	memcpy(controller_path, RUMINATION_DEBUGGER_CONTROLLER_PATH, len);
	char *argv[] = {
		controller_path,
		NULL
	};
	gint child_stdout;
	if( !g_spawn_async_with_pipes(
		NULL,
		argv,
		NULL,
		(GSpawnFlags) 0,
		(GSpawnChildSetupFunc) &setup_env,
		NULL,
		&rumination->child_pid,
		NULL,
		&child_stdout,
		NULL,
		err)
	) {
		return 0;
	}
	return child_stdout;
}

__attribute__((destructor))
static void rumination_destroy_atexit() {
	// TODO: Error handling
	rumination_destroy(NULL);
}

static int read_child_port( gint child_stdout ) {
	int port;
	size_t nb_read = 0;
	uint8_t *port_ptr = (uint8_t *) &port;

	do {
		ssize_t count = read(child_stdout, port_ptr, sizeof(int) - nb_read);
		if( count == -1 ) {
			// TODO: Error handling
			g_assert(false);
		}
		nb_read += count;
		port_ptr += count;
		g_assert(nb_read > 0);
		g_assert(nb_read <= sizeof(int));
	} while( nb_read != sizeof(int) );

	close(child_stdout);
	return port;
}

bool rumination_init( int *argc, char *argv[], GError **err ) {
	// TODO Thread safety
	if( rumination != NULL ) return true;
	rumination = new Rumination();

	gint child_stdout = fork_child(err);
	// TODO: Check for errors

	int port = read_child_port(child_stdout);

	rumination->communicator = Ice::initialize(*argc, argv);
	char *proxy_str = g_strdup_printf("DebuggerFactory:default -h 127.0.0.1 -p %d", port);
	printf("Connecting to proxy: \"%s\"\n", proxy_str);
	auto factory_proxy = rumination->communicator->stringToProxy(proxy_str);
	g_free(proxy_str);
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
	if( rumination == NULL ) return true;
	rumination->factory->shutdown();
	rumination->communicator->destroy();
	// TODO: Check child return code
	waitpid(rumination->child_pid, NULL, 0);
	g_spawn_close_pid(rumination->child_pid);
	delete rumination;
	rumination = NULL;

	return true;
}


void rumination_begin_get_type_by_variable_name( const char *varname, GError **err ) {
	g_assert(rumination->arp == 0);
	rumination->arp = rumination->debugger->begin_getTypeByVariableName(varname, gettid());
}

RType *rumination_end_get_type_by_variable_name( GError **err ) {
	g_assert(rumination->arp != 0);
	Ruminate::TypePrx t(rumination->debugger->end_getTypeByVariableName(rumination->arp));
	rumination->arp = 0;
	return r_type_new(t, err);
}

RFrameList *rumination_backtrace( GError **err ) {
	Ice::AsyncResultPtr arp(rumination->debugger->begin_getBacktrace(gettid()));
	rumination_hit_breakpoint();
	RFrameList *ret = r_frame_list_new(rumination->debugger->end_getBacktrace(arp), err);
	return ret;
}

G_END_DECLS
