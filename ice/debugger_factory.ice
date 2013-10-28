#ifndef _DEBUGGER_FACTORY_ICE_
#define _DEBUGGER_FACTORY_ICE_

#include <Ice/Identity.ice>
#include <debugger.ice>

module RuminateBackend {
	struct DebuggerFactoryOptions {
		string exename;
		long pid;
		long breakpointAddress;
	};

	interface DebuggerFactory {
		Debugger *create( DebuggerFactoryOptions options, Ice::Identity cbid );
		void shutdown() throws RuminateException;
	};
};

#endif
