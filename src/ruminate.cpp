#include <exception>
#include <sstream>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

#include <IceUtil/UUID.h>
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
#include "ruminate/pointer_type.h"
#include "ruminate/frame.h"
#include "ruminate/ruminate.h"

#include "private/common.h"
#include "private/string.h"
#include "private/errors.h"
#include "private/gettid.h"
#include "private/type.h"
#include "private/frame.h"
#include "private/ruminate.h"

static Ruminate *ruminate;

__attribute__((noinline))
void ruminate_hit_breakpoint() RUMINATE_NOEXCEPT {
	// We try really hard to not be optimized away.
	asm("");
}

G_BEGIN_DECLS

static gint fork_child( GError **err ) RUMINATE_NOEXCEPT {
	size_t len = strlen(RUMINATE_DEBUGGER_CONTROLLER_PATH) + 1;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wvla-extension"
	char controller_path[len];
#pragma clang diagnostic pop
	memcpy(controller_path, RUMINATE_DEBUGGER_CONTROLLER_PATH, len);
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
		NULL,
		NULL,
		&ruminate->child_pid,
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
static void ruminate_destroy_atexit() RUMINATE_NOEXCEPT {
	// TODO: Error handling
	ruminate_destroy(NULL);
}

static int read_child_port( gint child_stdout, GError **error ) RUMINATE_NOEXCEPT {
	int port;
	size_t nb_read = 0;
	uint8_t *port_ptr = (uint8_t *) &port;

	do {
		ssize_t count = read(child_stdout, port_ptr, sizeof(int) - nb_read);
		if( count == -1 ) {
			r_perror(error, "read");
			return -1;
		}
		nb_read += count;
		port_ptr += count;
		if( nb_read == 0 ) {
			g_set_error_literal(
				error,
				RUMINATE_ERROR,
				RUMINATE_ERROR_SHORT_READ,
				"Couldn't read listening port from child"
			);
			return -1;
		}
		g_assert(nb_read <= sizeof(int));
	} while( nb_read != sizeof(int) );

	close(child_stdout);
	return port;
}

R_STATIC_ASSERT(sizeof(::Ice::Long) >= sizeof(pid_t) && "A pid cannot fit in a long!");
R_STATIC_ASSERT(sizeof(::Ice::Long) >= sizeof(size_t) && "A pointer cannot fit in a long!");

bool ruminate_init( int *argc, char *argv[], GError **error ) RUMINATE_NOEXCEPT {
	GError *err = NULL;
	RuminateBackend::DebuggerFactoryOptions opts;
	char *proxy_str;
	Ice::ObjectPrx factory_proxy;
	gint child_stdout;
	int port;
#if 0
	Ice::Identity cbid;
#endif
	Ice::PropertiesPtr props;
	Ice::InitializationData id;

	// TODO Thread safety
	if( ruminate != NULL ) return true;
	ruminate = g_new(Ruminate, 1);

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

	// TODO: Move all this checked C++ code into it's own method which can throw
	// TODO: Remove ice arguments from argc.
	if( !gxx_call(props = Ice::createProperties(*argc, argv), error) )
		goto error_ice_create_properties;
	if( !gxx_call(props->setProperty("Ice.ACM.Client", "0"), error) )
		goto error_ice_set_property;
	if( !gxx_call(id.properties = props, error) )
		goto error_ice_assign_properties;

	if( !gxx_call(ruminate->communicator = Ice::initialize(id), error) )
		goto error_ice_initialize;

	proxy_str = g_strdup_printf("DebuggerFactory:default -h 127.0.0.1 -p %d", port);

	if( !gxx_call(factory_proxy = ruminate->communicator->stringToProxy(proxy_str), error) )
		goto error_communicator_stringToProxy;

	if( !gxx_call(ruminate->factory = RuminateBackend::DebuggerFactoryPrx::checkedCast(factory_proxy), error) )
		goto error_checkedCast;

#if 0
	// TODO: Handle exceptions
	if( !gxx_call(cbid = init_callbacks(ruminate->communicator, ruminate->factory), error) )
		goto error_init_callbacks;
#endif

	opts.exename = argv[0];
	opts.pid = getpid();
	opts.breakpointAddress = (::Ice::Long) &ruminate_hit_breakpoint;

#if 0
	if( !gxx_call(ruminate->debugger = ruminate->factory->create(opts, cbid), error) )
		goto error_factory_create;
#else
	if( !gxx_call(ruminate->debugger = ruminate->factory->create(opts, Ice::Identity()), error) )
		goto error_factory_create;
#endif

	atexit(ruminate_destroy_atexit);

	g_free(proxy_str);

	return true;

error_factory_create:
#if 0
error_init_callbacks:
#endif
error_checkedCast:
error_communicator_stringToProxy:
	g_free(proxy_str);
error_ice_initialize:
error_ice_assign_properties:
error_ice_set_property:
error_ice_create_properties:
error_read_child_port:
error_fork_child:
	// TODO: Resource cleanup
	return false;
}

bool ruminate_destroy( GError **error ) RUMINATE_NOEXCEPT {
	if( ruminate == NULL ) return true;
	// TODO: Error check these
	(void) gxx_call(ruminate->factory->shutdown(), error);
	(void) gxx_call(ruminate->communicator->destroy(), error);
	// TODO: Check child return code
	waitpid(ruminate->child_pid, NULL, 0);
	g_spawn_close_pid(ruminate->child_pid);
	g_free(ruminate);
	ruminate = NULL;

	return true;
}

RFrameList *ruminate_backtrace( GError **error ) RUMINATE_NOEXCEPT {
	Ice::AsyncResultPtr arp;
	if( !gxx_call(arp = ruminate->debugger->begin_getBacktrace(gettid()), error) )
		return NULL;
	ruminate_hit_breakpoint();
	RuminateBackend::FrameList fl = ruminate->debugger->end_getBacktrace(arp);
	return r_frame_list_new(fl, error);
}

RType *ruminate_get_type_by_variable_name( const char *varname, GError **error ) RUMINATE_NOEXCEPT {
	g_assert(ruminate->arp == 0);
	if( !gxx_call(ruminate->arp = ruminate->debugger->begin_getTypeByVariableName(varname, gettid()), error) )
		return NULL;

	ruminate_hit_breakpoint();

	RuminateBackend::TypePrx t;
	if( !gxx_call(t = ruminate->debugger->end_getTypeByVariableName(ruminate->arp), error) ) {
		// TODO: Cleanup
		return NULL;
	}
	ruminate->arp = 0;
	// TODO: Type check
	return r_type_new(t, error);
}

RString *ruminate_get_function_name( void *addr, GError **error ) RUMINATE_NOEXCEPT {
	std::string name;
	if( !gxx_call(name = ruminate->debugger->getFunctionName((::Ice::Long) addr), error) )
		return NULL;

	return r_string_new_cxx(name);
}

G_END_DECLS
