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
#include "ice/debugee.h"
#include "ice/frame.h"
#include "ice/type.h"

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/type.h"
#include "ruminate/pointer_type.h"
#include "ruminate/frame.h"
#include "ruminate/rumination.h"

#include "private/common.h"
#include "private/gettid.h"
#include "private/memory.h"
#include "private/type.h"
#include "private/frame.h"
#include "private/rumination.h"

#define RUMINATION_DEBUGGER_CONTROLLER_PATH "./src/dbgsvr.py"

static Rumination *rumination;

static Ice::Identity init_callbacks( Ice::CommunicatorPtr &, Ruminate::DebuggerFactoryPrx & );

G_BEGIN_DECLS

__attribute__((noinline))
void rumination_hit_breakpoint() RUMINATE_NOEXCEPT {
	// We try really hard to not be optimized away.
	asm("");
}

static void setup_env() RUMINATE_NOEXCEPT {
	// TODO: Remove this
	setenv("PYTHONPATH", "ice:src:.:/usr/local/lib/python2.7/site-packages", true);
}

static gint fork_child( GError **err ) RUMINATE_NOEXCEPT {
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
static void rumination_destroy_atexit() RUMINATE_NOEXCEPT {
	// TODO: Error handling
	rumination_destroy(NULL);
}

static int read_child_port( gint child_stdout, GError **error ) RUMINATE_NOEXCEPT {
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

G_STATIC_ASSERT(sizeof(::Ice::Long) >= sizeof(pid_t) && "A pid cannot fit in a long!");
G_STATIC_ASSERT(sizeof(::Ice::Long) >= sizeof(size_t) && "A pointer cannot fit in a long!");

bool rumination_init( int *argc, char *argv[], GError **error ) RUMINATE_NOEXCEPT {
	GError *err = NULL;
	Ruminate::DebuggerFactoryOptions opts;
	char *proxy_str;
	Ice::ObjectPrx factory_proxy;
	gint child_stdout;
	int port;
	Ice::Identity cbid;
	Ice::PropertiesPtr props;
	Ice::InitializationData id;

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

	// TODO: Handle exceptions
	// TODO: Remove ice arguments from argc.
	props = Ice::createProperties(*argc, argv);
	props->setProperty("Ice.ACM.Client", "0");
	id.properties = props;

	if( !gxx_call(rumination->communicator = Ice::initialize(id), error) )
		goto error_ice_initialize;

	proxy_str = g_strdup_printf("DebuggerFactory:default -h 127.0.0.1 -p %d", port);
	printf("Connecting to proxy: \"%s\"\n", proxy_str);

	if( !gxx_call(factory_proxy = rumination->communicator->stringToProxy(proxy_str), error) )
		goto error_communicator_stringToProxy;

	if( !gxx_call(rumination->factory = Ruminate::DebuggerFactoryPrx::checkedCast(factory_proxy), error) )
		goto error_checkedCast;

	// TODO: Handle exceptions
	cbid = init_callbacks(rumination->communicator, rumination->factory);

	opts.exename = argv[0];
	opts.pid = getpid();
	opts.breakpointAddress = (::Ice::Long) &rumination_hit_breakpoint;

	if( !gxx_call(rumination->debugger = rumination->factory->create(opts, cbid), error) )
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

bool rumination_destroy( GError **error ) RUMINATE_NOEXCEPT {
	if( rumination == NULL ) return true;
	// TODO: Error check these
	(void) gxx_call(rumination->factory->shutdown(), error);
	(void) gxx_call(rumination->communicator->destroy(), error);
	// TODO: Check child return code
	waitpid(rumination->child_pid, NULL, 0);
	g_spawn_close_pid(rumination->child_pid);
	g_free(rumination);
	rumination = NULL;

	return true;
}


bool rumination_begin_get_type_by_variable_name( const char *varname, GError **error ) RUMINATE_NOEXCEPT {
	g_assert(rumination->arp == 0);
	return gxx_call(rumination->arp = rumination->debugger->begin_getTypeByVariableName(varname, gettid()), error);
}

RType *rumination_end_get_type_by_variable_name( void *mem, GError **error ) RUMINATE_NOEXCEPT {
	g_assert(rumination->arp != 0);
	Ruminate::TypePrx t;
	if( !gxx_call(t = rumination->debugger->end_getTypeByVariableName(rumination->arp), error) ) {
		// TODO: Cleanup
		return NULL;
	}
	rumination->arp = 0;
	RMemory *rmem = r_memory_new(mem, error);
	if( rmem == NULL ) {
		// TODO: Cleanup
		return NULL;
	}
	// TODO: Type check
	RPointerType *pt = (RPointerType *) r_type_new(t, rmem, &mem, error);
	if( pt == NULL ) return NULL;
	RType *ret = r_pointer_type_pointee(pt, error);
	r_type_unref((RType *) pt);
	return ret;
}

RFrameList *rumination_backtrace( GError **error ) RUMINATE_NOEXCEPT {
	Ice::AsyncResultPtr arp;
	if( !gxx_call(arp = rumination->debugger->begin_getBacktrace(gettid()), error) )
		return NULL;
	rumination_hit_breakpoint();
	Ruminate::FrameList fl = rumination->debugger->end_getBacktrace(arp);
	return r_frame_list_new(fl, error);
}

G_END_DECLS

class DebugeeImpl : public Ruminate::Debugee {
public:
	void stop( const Ice::Current & = Ice::Current() ) {
		fprintf(stderr, "Hello World!\n");
		rumination_hit_breakpoint();
	}
};

static Ice::Identity init_callbacks( Ice::CommunicatorPtr &communicator, Ruminate::DebuggerFactoryPrx &proxy ) {
	Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("");
	Ice::Identity ident;
	ident.name = IceUtil::generateUUID();
	ident.category = "";
	Ruminate::DebugeePtr cb = new DebugeeImpl();
	adapter->add(cb, ident);
	adapter->activate();
	proxy->ice_getConnection()->setAdapter(adapter);
	return ident;
}
