#include "ruminate/init.h"
#include "ruminate/errors.h"

#include "private/gettid.h"

#include <Ice/Ice.h>
#include "debugger_factory.h"
#include "debugger.h"

#include <glib.h>

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <new>

using namespace Ruminate;

struct _RDebugger {
	Ice::CommunicatorPtr communicator;
	DebuggerPrx debugger;
};

static void sentinel_fn() {
	printf("Inside sentinel_fn\n");
}

__attribute__((noreturn))
static void die( const char *fn, int line ) {
	fprintf(stderr, "die(): %s:%d\n", fn, line);
	abort();
}
#define die() die(__FILE__, __LINE__)
#define die_unless(cond) if( !cond ) { die(); } else {}

RDebugger *r_debugger_new( GError **err ) {
	// TODO: Handle errors properly
	try {
		RDebugger *ret = new RDebugger();

		int argv;
		char *argc[0];
		ret->communicator = Ice::initialize(argv, argc);
		Ice::ObjectPrx base = ret->communicator->stringToProxy("DebuggerFactory:default -p 1024");
		DebuggerFactoryPrx factory = DebuggerFactoryPrx::checkedCast(base);
		die_unless(factory);
		DebuggerFactoryOptions opts;
		opts.exename = "./test/ruminate.exe";
		opts.pid = getpid();
		opts.breakpointAddress = (::Ice::Long) &sentinel_fn;
		ret->debugger = factory->create(opts);
		die_unless(ret->debugger);

		auto fn = sentinel_fn;
		(void) fn;

		printf("Calling begin_getTypeByVariableName\n");
		Ice::AsyncResultPtr arp = ret->debugger->begin_getTypeByVariableName("fn", gettid());
		printf("Finished calling begin_getTypeByVariableName\n");

		sentinel_fn();

		printf("Calling end_getTypeByVariableName\n");
		TypePrx t = ret->debugger->end_getTypeByVariableName(arp);
		printf("Finished calling end_getTypeByVariableName\n");

		printf("%s\n", t->getName().c_str());
		abort();
		factory->shutdown();
		return ret;
	} catch( std::bad_alloc &ex ) {
		die();
	} catch( const Ruminate::SBErrorException &ex ) {
		std::cerr << ex << std::endl;
		std::cerr << ex.message << std::endl;
		die();
	} catch( const Ice::Exception &ex ) {
		fprintf(stderr, "Hello?\n");
		std::cerr << ex << std::endl;
		die();
	}
}

void r_debugger_delete( RDebugger *rum ) {
	rum->communicator->destroy();
	delete rum;
}
