#include "ruminate/rumination.h"
#include "ruminate/errors.h"
#include "ruminate/init.h"

#include "ruminate/type.h"
#include "private/type.h"

#include <glib.h>
#include <lldb/API/LLDB.h>
#include <lldb/API/SBStream.h>

#include <boost/interprocess/anonymous_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

#include <cassert>

#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>
#include <pthread.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <limits.h>

#ifdef __linux__
#include <sys/syscall.h>
#else
#error Don't know how to get tid on this platform
#endif

using namespace boost::interprocess;
using namespace lldb;

struct Rumination {
	SBDebugger debugger;
	SBTarget target;
#if 0
	SBProcess process;
#endif
};

static void log_output_callback( const char *msg, void *baton ) {
	g_message("%s", msg);
}

Rumination *rumination_new( const char *exename, GError **err ) {
	::pid_t pid = fork();
	Type *ret;
	if( pid == -1 ) {
	
}

Rumination *rumination_new( const char *exename, GError **err ) {
	SBError sberr;
	SBListener listener;
	const char *all[] = { "all", NULL };

	Rumination *rum = g_slice_new(Rumination);
	new (rum) Rumination();

	rum->debugger = SBDebugger::Create(false, log_output_callback, NULL);
	if( !rum->debugger.IsValid() ) {
		g_set_error(
			err,
			RUMINATE_ERROR,
			RUMINATE_ERROR_SB_INVALID,
			"SBDebugger instance invalid"
		);
		goto debugger_invalid;
	}

#if 0
	rum->debugger.EnableLog("dwarf", all);
	rum->debugger.EnableLog("kdp-remote", all);
	rum->debugger.EnableLog("gdb-remote", all);
	rum->debugger.EnableLog("lldb", all);
#endif

	rum->target = rum->debugger.CreateTarget(exename);
	if( !rum->target.IsValid() ) {
		g_set_error(
			err,
			RUMINATE_ERROR,
			RUMINATE_ERROR_SB_INVALID,
			"SBTarget instance invalid"
		);
		goto target_invalid;
	}

#if 0
	rum->process = rum->target.AttachToProcessWithID(listener, getpid(), sberr);
	if( !sberr.Success() ) {
		g_set_error_literal(
			err,
			RUMINATE_ERROR,
			RUMINATE_ERROR_LLDB_ERROR,
			sberr.GetCString()
		);
		goto process_fail;
	}
	if( !rum->process.IsValid() ) {
		g_set_error(
			err,
			RUMINATE_ERROR,
			RUMINATE_ERROR_SB_INVALID,
			"SBProcess instance invalid"
		);
		goto process_invalid;
	}
#endif

	return rum;

process_invalid:
process_fail:
target_invalid:
	SBDebugger::Destroy(rum->debugger);
debugger_invalid:
	rum->~Rumination();
	g_slice_free(Rumination, rum);

	return NULL;
}

void rumination_delete( Rumination **rum ) {
	g_return_if_fail(rum != NULL && *rum != NULL);

	Rumination *r = *rum;
#if 0
	if( r->process.GetState() != eStateDetached ) r->process.Detach();
#endif
	SBDebugger::Destroy(r->debugger);
	r->~Rumination();
	g_slice_free(Rumination, r);
	*rum = NULL;
}

Type *rumination_find_type( Rumination *rum, const char *type_name, GError **err ) {
	for( uint32_t i = 0; i < rum->target.GetNumModules(); i++ ) {
		SBModule module(rum->target.GetModuleAtIndex(i));
		if( !module.IsValid() ) continue;

		SBTypeList type_list(module.FindTypes(type_name));
		if( !type_list.IsValid() ) continue;

		for( uint32_t x = 0; x < type_list.GetSize(); x++ ) {
			SBType type(type_list.GetTypeAtIndex(x));
			if( !type.IsValid() ) continue;

			return type_new(type, err);
		}
	}

	return NULL;
}

#if 0
Type *rumination_get_type( Rumination *rum, void *obj, GError **err ) {
	uint64_t tid;
#ifdef __linux__
	int _tid = syscall(SYS_gettid);
	if( _tid < 0 ) {
		g_set_error_literal(err, RUMINATE_ERROR, RUMINATE_ERROR_ERRNO, strerror(errno));
		return NULL;
	}
	tid = _tid;
#endif
	SBThread thread = rum->process.GetThreadByID(tid);

	SBStream strm;
	strm.RedirectToFileHandle(stdout, false);
	thread.GetDescription(strm);

	return NULL;
}
#endif

static bool breakpoint_callback( void *baton, SBProcess &process, SBThread &thread, SBBreakpointLocation &location ) {
	printf("Breakpoint hit!");

	process.GetTarget().BreakpointDelete(location.GetID());
	return false;
}

static Type *rumination_get_type_parent( Rumination *rum, GError **err ) {
	printf("rumination_get_type_parent\n");

	return NULL;
}

static int rumination_get_type_child( Rumination *rum, interprocess_mutex *mutex, ::pid_t parent_pid ) {
	SBStream out;
	SBError err;
	out.RedirectToFileHandle(stderr, false);

	rum->debugger.SetAsync(false);
	assert(!rum->debugger.GetAsync());

	SBListener debugger_listener = rum->debugger.GetListener();
	SBProcess process(rum->target.AttachToProcessWithID(debugger_listener, parent_pid, err));
	assert(err.Success());
	assert(process.IsValid());
	process.GetDescription(out); out.Printf("\n");

	SBBreakpoint breakpoint(rum->target.BreakpointCreateByAddress((addr_t) rumination_get_type_parent));
	assert(breakpoint.IsValid());
	breakpoint.GetDescription(out); out.Printf("\n");

	assert(breakpoint.GetNumLocations() == 1);
	SBBreakpointLocation location(breakpoint.GetLocationAtIndex(0));
	assert(location.IsValid());
	assert(location.IsEnabled());

	//mutex->unlock();
	printf("Mutex unlocked\n");
	printf("state = %d\n", process.GetState());
	assert(process.Continue().Success());
	printf("Process continued\n");

	assert(rum->target.BreakpointDelete(breakpoint.GetID()));
	printf("Breakpoint deleted\n");

	assert(process.Detach().Success());
	printf("Process detached\n");

	return EXIT_SUCCESS;
}

#if 0
static int rumination_get_type_child( Rumination *rum, interprocess_mutex *mutex, ::pid_t parent_pid ) {
	SBError err;
	SBListener listener("rumination-listener");
	bool buf = true;
	SBEvent event;
	SBStream out2;
	uint32_t ret;
	SBBroadcaster broadcaster;
	bool ret_b;
	SBBreakpoint breakpoint;
	SBListener listener2(rum->debugger.GetListener());

	//SBBreakpoint breakpoint = rum->target.BreakpointCreateByAddress((addr_t) rumination_get_type_parent);
	breakpoint = rum->target.BreakpointCreateByName("rumination_get_type_parent");
	//breakpoint.SetCallback(breakpoint_callback, NULL);

	SBProcess process = rum->target.AttachToProcessWithID(listener2, parent_pid, err);
	if( !err.Success() ) goto process_fail;

	broadcaster = process.GetBroadcaster();
	ret = broadcaster.AddListener(listener, SBProcess::eBroadcastBitStateChanged);
	if( ret != SBProcess::eBroadcastBitStateChanged ) {
		fprintf(stderr, "Unable to add listener to SBBroadcaster\n");
		return EXIT_FAILURE;
	}

	mutex->unlock();
	printf("Mutex unlocked\n");

	ret_b = listener.WaitForEventForBroadcasterWithType(
		UINT32_MAX,
		broadcaster,
		SBProcess::eBroadcastBitStateChanged,
		event
	);
	if( ret_b ) {
		//listener.PeekAtNextEvent(event);
		out2.RedirectToFileHandle(stderr, false);
		event.GetDescription(out2);
		out2.Printf("\n");
	} else {
		printf("Returned false!\n");
	}

	printf("state = %d\n", process.GetState());

	//process.Continue();
	//printf("Process continued\n");
	rum->target.BreakpointDelete(breakpoint.GetID());
	printf("Breakpoint deleted\n");

	process.Detach();
	printf("Process detached\n");

	return EXIT_SUCCESS;

process_fail:
	SBStream out;
	out.RedirectToFileHandle(stderr, false);
	err.GetDescription(out);
	out.Printf("\n");

	return EXIT_FAILURE;
}
#endif

Type *rumination_get_type( Rumination *rum, void *obj, GError **err ) {
	::pid_t parent_pid = getpid();
	uint64_t tid;
#ifdef __linux__
	int _tid = syscall(SYS_gettid);
	if( _tid < 0 ) {
		g_set_error_literal(err, RUMINATE_ERROR, RUMINATE_ERROR_ERRNO, strerror(errno));
		return NULL;
	}
	tid = _tid;
#endif

	mapped_region mutex_shm(anonymous_shared_memory(sizeof(interprocess_mutex)));
	interprocess_mutex *mutex = (interprocess_mutex *) mutex_shm.get_address();
	new (mutex) interprocess_mutex();
	mutex->lock();

	//SBBreakpoint breakpoint = rum->target.BreakpointCreateByAddress((addr_t) rumination_get_type_parent);
	//SBBreakpoint breakpoint = rum->target.BreakpointCreateByName("rumination_get_type_parent");
	//if( !breakpoint.IsValid() ) {
	//	g_set_error(
	//		err,
	//		RUMINATE_ERROR,
	//		RUMINATE_ERROR_SB_INVALID,
	//		"SBBreakpoint instance invalid"
	//	);
	//	return NULL;
	//}

	::pid_t pid = fork();
	Type *ret;
	if( pid == -1 ) {
		g_set_error_literal(
			err,
			RUMINATE_ERROR,
			RUMINATE_ERROR_ERRNO,
			strerror(errno) /* TODO: use strerror_r */
		);
		return NULL;
	} else if( pid == 0 ) {
		int ret = rumination_get_type_child(rum, mutex, parent_pid);
		rumination_delete(&rum);
		ruminate_terminate();
		exit(ret);
	}

	printf("Locking mutex\n");
	//rum->target.BreakpointDelete(breakpoint.GetID());
	//mutex->lock();
	sleep(3);
	printf("Locking mutex returned\n");
	ret = rumination_get_type_parent(rum, err);

	if( waitpid(pid, NULL, 0) == -1 ) {
		g_set_error_literal(
			err,
			RUMINATE_ERROR,
			RUMINATE_ERROR_ERRNO,
			strerror(errno)
		);
		return NULL;
	}

	return ret;
}
