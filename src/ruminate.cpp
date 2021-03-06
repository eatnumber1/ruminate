#include "config.h"

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

#include "private/ice.h"
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
	const char *controller_path = g_getenv("RUMINATE_DEBUGGER_CONTROLLER");
	size_t len;
	if( controller_path == NULL ) {
		len = strlen(RUMINATE_DEBUGGER_CONTROLLER_PATH) + 1;
	} else {
		len = strlen(controller_path) + 1;
	}
	char controller_path_ary[len];
	if( controller_path == NULL ) {
		memcpy(controller_path_ary, RUMINATE_DEBUGGER_CONTROLLER_PATH, len);
	} else {
		memcpy(controller_path_ary, controller_path, len);
	}
	char *argv[] = {
		controller_path_ary,
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
		g_assert(count >= 0);
		nb_read += (size_t) count;
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

static void setup_ice( const char *prgname ) {
	int argc = 1;

	size_t prgname_len = strlen(prgname) + 1;
	char prgname_vla[prgname_len];
	memcpy(prgname_vla, prgname, prgname_len);
	char *argv[] = { prgname_vla, NULL };

	Ice::PropertiesPtr props = Ice::createProperties(argc, argv);
	props->setProperty("Ice.ACM.Client", "0");
	Ice::InitializationData id;
	id.properties = props;
	ruminate->communicator = Ice::initialize(id);
}

static void create_factory( int port ) {
	struct AutoHeapString {
		~AutoHeapString() throw() {
			g_free(value);
		}

		AutoHeapString( int port ) {
			value = g_strdup_printf("DebuggerFactory:default -h 127.0.0.1 -p %d", port);
		}

		char *value;
	} proxy_str(port);

	Ice::ObjectPrx factory_proxy = ruminate->communicator->stringToProxy(proxy_str.value);
	ruminate->factory = RuminateBackend::DebuggerFactoryPrx::checkedCast(factory_proxy);
}

static bool create_debugger( const char *prgname, GError **error ) RUMINATE_NOEXCEPT {
	RuminateBackend::DebuggerFactoryOptions opts;
	opts.exename = prgname;
	opts.pid = getpid();
	opts.breakpointAddress = (::Ice::Long) &ruminate_hit_breakpoint;

#if 0
	if( !gxx_call(ruminate->debugger = ruminate->factory->create(opts, cbid), error) )
		return false;
#else
	if( !gxx_call(ruminate->debugger = ruminate->factory->create(opts, Ice::Identity()), error) )
		return false;
#endif
	return true;
}

R_STATIC_ASSERT(sizeof(::Ice::Long) >= sizeof(pid_t) && "A pid cannot fit in a long!");
R_STATIC_ASSERT(sizeof(::Ice::Long) >= sizeof(size_t) && "A pointer cannot fit in a long!");

bool ruminate_init( const char *prgname, GError **error ) RUMINATE_NOEXCEPT {
	GError *err = NULL;
	gint child_stdout;
	int port;
#if 0
	Ice::Identity cbid;
#endif

	// TODO Thread safety
	if( ruminate != NULL ) return true;
	ruminate = g_new(Ruminate, 1);
	new (ruminate) Ruminate();

	if( prgname == NULL ) prgname = g_get_prgname();
	if( prgname == NULL ) {
		g_set_error_literal(
			error,
			RUMINATE_ERROR,
			RUMINATE_ERROR_NO_PRGNAME,
			"Unable to determine executable name"
		);
		return false;
	}

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

	if( !gxx_call(setup_ice(prgname), error) )
		goto error_setup_ice;

	if( !gxx_call(create_factory(port), error) )
		goto error_create_factory;

#if 0
	// TODO: Handle exceptions
	if( !gxx_call(cbid = init_callbacks(ruminate->communicator, ruminate->factory), error) )
		goto error_init_callbacks;
#endif

	if( !create_debugger(prgname, error) ) goto error_create_debugger;

	atexit(ruminate_destroy_atexit);

	return true;

error_create_debugger:
#if 0
error_init_callbacks:
#endif
error_create_factory:
error_setup_ice:
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
	ruminate->~Ruminate();
	g_free(ruminate);
	ruminate = NULL;

	return true;
}

RFrameList *ruminate_backtrace( GError **error ) RUMINATE_NOEXCEPT {
	Ice::AsyncResultPtr arp;
	if( !gxx_call(arp = ruminate->debugger->begin_getBacktrace((::Ice::Long) gettid()), error) )
		return NULL;
	ruminate_hit_breakpoint();
	RuminateBackend::FrameList fl = ruminate->debugger->end_getBacktrace(arp);
	return r_frame_list_new(fl, error);
}

RType *ruminate_get_type_by_variable_name( const char *varname, GError **error ) RUMINATE_NOEXCEPT {
	g_assert(ruminate->arp == 0);
	if( !gxx_call(ruminate->arp = ruminate->debugger->begin_getTypeByVariableName(varname, (::Ice::Long) gettid()), error) )
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

GPtrArray *ruminate_get_types_by_name( const char *name, GError **error ) RUMINATE_NOEXCEPT {
	RuminateBackend::TypeList types;
	if( !gxx_call(types = ruminate->debugger->getTypesByName(name), error) )
		return NULL;

	// TODO: size here could overflow
	GPtrArray *ret = g_ptr_array_new_full((guint) types.size(), (GDestroyNotify) r_type_unref);
	for( RuminateBackend::TypeList::iterator t = types.begin(); t != types.end(); t++ ) {
		RType *type = r_type_new(*t, error);
		if( type == NULL ) goto error_type_new;
		g_ptr_array_add(ret, type);
	}

	return ret;
error_type_new:
	g_ptr_array_unref(ret);
	return NULL;
}

G_END_DECLS
