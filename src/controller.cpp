#include <glib.h>
#include <glib/gprintf.h>

#include "private/debugger_impl.h"

#include <Ice/Ice.h>
#include "ice/debugger.h"

#include <stdlib.h>

using namespace Ruminator;

int main( int argc, char **argv ) {
	if( g_getenv("RUMINATE_CHILD_PROC") == NULL ) {
		g_fprintf(stderr, "This program should not be called directly\n");
		exit(EXIT_FAILURE);
	}

	Ice::CommunicatorPtr communicator = Ice::initialize(argc, argv);
	Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapterWithEndpoints("DebuggerAdapter", "default -p 1024");
	Ice::ObjectPtr object = new DebuggerImpl();
	adapter->add(object, communicator->stringToIdentity("Debugger"));
	adapter->activate();
	communicator->waitForShutdown();
	communicator->destroy();

	return EXIT_SUCCESS;
}
