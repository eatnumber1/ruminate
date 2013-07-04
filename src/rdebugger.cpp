#include "private/rdebugger.h"

#include "ruminate/errors.h"

#include <Ice/Ice.h>
#include "ice/debugger_factory.h"
#include "ice/debugger.h"

#include <glib.h>

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <new>

using namespace Ruminate;

struct _RDebugger {
	Ice::CommunicatorPtr communicator;
	DebuggerFactoryPrx factory;
};

__attribute__((noreturn))
static void die( const char *fn, int line ) {
	fprintf(stderr, "die(): %s:%d\n", fn, line);
	abort();
}
#define die() die(__FILE__, __LINE__)
#define die_unless(cond) if( !cond ) { die(); } else {}

// TODO: Reference count an internal ref to the DebuggerFactoryPrx

RDebugger *r_debugger_new( int argc, char *argv[], GError **err ) {
	// TODO: Handle errors properly
	try {
		RDebugger *ret = new RDebugger();

		ret->communicator = Ice::initialize(argc, argv);
		ret->factory = DebuggerFactoryPrx::checkedCast(ret->communicator->stringToProxy("DebuggerFactory:default -p 1024"));
		die_unless(ret->factory);

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

void r_debugger_delete( RDebugger *debugger ) {
	debugger->factory->shutdown();
	debugger->communicator->destroy();
	delete debugger;
}

Ruminate::DebuggerFactoryPrx &rdebugger_get_factory( RDebugger *debugger ) {
	return debugger->factory;
}
