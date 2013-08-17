#include <functional>
#include <memory>
#include <string>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdint>

#include <sys/types.h>
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

#include <Ice/Ice.h>
#include "ice/debugger_factory.h"
#include "ice/debugger.h"
#include "ice/frame.h"
#include "ice/type.h"

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/type.h"
#include "ruminate/frame.h"
#include "ruminate/rumination.h"

#define _RUMINATE_CPP_

#include "private/common.h"
#include "private/gettid.h"
#include "private/type.h"
#include "private/frame.h"
#include "private/rumination.h"

#define RUMINATION_DEBUGGER_CONTROLLER_PATH "./src/dbgsvr.py"

static Rumination *rumination;

template gxx_call_proto(Ice::CommunicatorPtr);
template gxx_call_proto(Ice::ObjectPrx);
template gxx_call_proto(Ice::AsyncResultPtr);
template gxx_call_proto(Ruminate::DebuggerFactoryPrx);
template gxx_call_proto(Ruminate::DebuggerPrx);

G_BEGIN_DECLS

__attribute__((noinline))
void rumination_hit_breakpoint() noexcept {
	// We try really hard to not be optimized away.
	asm("");
}

static void setup_env() noexcept {
	// TODO: Remove this
	setenv("PYTHONPATH", "ice:src:.:/usr/local/lib/python2.7/site-packages", true);
}

static gint fork_child( GError **err ) noexcept {
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
static void rumination_destroy_atexit() noexcept {
	// TODO: Error handling
	rumination_destroy(NULL);
}

static int read_child_port( gint child_stdout, GError **error ) noexcept {
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

bool rumination_init( int *argc, char *argv[], GError **error ) noexcept {
	GError *err = NULL;
	Ruminate::DebuggerFactoryOptions opts;
	char *proxy_str;
	Ice::ObjectPrx factory_proxy;
	gint child_stdout;
	int port;

	// TODO Thread safety
	if( rumination != NULL ) return true;
	rumination = g_new(Rumination, 1);

	child_stdout = fork_child(&err);
	if( err != NULL ) {
		g_propagate_error(error, err);
		goto error_fork_child;
	}

	port = read_child_port(child_stdout, &err);
	if( err != NULL ) {
		g_propagate_error(error, err);
		goto error_read_child_port;
	}

	if( !gxx_call<Ice::CommunicatorPtr>([argc, argv](){ return Ice::initialize(*argc, argv); }, &rumination->communicator, error) )
		goto error_ice_initialize;

	proxy_str = g_strdup_printf("DebuggerFactory:default -h 127.0.0.1 -p %d", port);
	printf("Connecting to proxy: \"%s\"\n", proxy_str);

	if( !gxx_call<Ice::ObjectPrx>([proxy_str](){ return rumination->communicator->stringToProxy(proxy_str); }, &factory_proxy, error) )
		goto error_communicator_stringToProxy;

	if( !gxx_call<Ruminate::DebuggerFactoryPrx>([&factory_proxy](){ return Ruminate::DebuggerFactoryPrx::checkedCast(factory_proxy); }, &rumination->factory, error) )
		goto error_checkedCast;

	opts.exename = argv[0];
	static_assert(sizeof(::Ice::Long) >= sizeof(pid_t), "A pid cannot fit in a long!");
	opts.pid = getpid();
	static_assert(sizeof(::Ice::Long) >= sizeof(size_t), "A pointer cannot fit in a long!");
	opts.breakpointAddress = (::Ice::Long) &rumination_hit_breakpoint;

	if( !gxx_call<Ruminate::DebuggerPrx>([&opts](){ return rumination->factory->create(opts); }, &rumination->debugger, error) )
		goto error_factory_create;

	atexit(rumination_destroy_atexit);

	g_free(proxy_str);

	return true;

error_factory_create:
error_checkedCast:
error_communicator_stringToProxy:
	g_free(proxy_str);
error_ice_initialize:
error_read_child_port:
error_fork_child:
	// TODO: Resource cleanup
	return false;
}

bool rumination_destroy( GError **error ) noexcept {
	if( rumination == NULL ) return true;
	// TODO: Error check these
	gxx_call([](){ rumination->factory->shutdown(); }, error);
	gxx_call([](){ rumination->communicator->destroy(); }, error);
	// TODO: Check child return code
	waitpid(rumination->child_pid, NULL, 0);
	g_spawn_close_pid(rumination->child_pid);
	g_free(rumination);
	rumination = NULL;

	return true;
}


bool rumination_begin_get_type_by_variable_name( const char *varname, GError **error ) noexcept {
	g_assert(rumination->arp == 0);
	return gxx_call<Ice::AsyncResultPtr>([varname](){ return rumination->debugger->begin_getTypeByVariableName(varname, gettid()); }, &rumination->arp, error);
}

RType *rumination_end_get_type_by_variable_name( GError **error ) noexcept {
	g_assert(rumination->arp != 0);
	Ruminate::TypePrx t;
	if( !gxx_call<Ruminate::TypePrx>([](){ return rumination->debugger->end_getTypeByVariableName(rumination->arp); }, &t, error) ) {
		// TODO: Cleanup
		return NULL;
	}
	rumination->arp = 0;
	return r_type_new(t, error);
}

RFrameList *rumination_backtrace( GError **error ) noexcept {
	Ice::AsyncResultPtr arp;
	if( !gxx_call<Ice::AsyncResultPtr>([](){ return rumination->debugger->begin_getBacktrace(gettid()); }, &arp, error) )
		return NULL;
	rumination_hit_breakpoint();
	return r_frame_list_new(rumination->debugger->end_getBacktrace(arp), error);
}

G_END_DECLS
