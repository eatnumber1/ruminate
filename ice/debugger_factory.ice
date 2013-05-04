#ifndef _DEBUGGER_FACTORY_ICE_
#define _DEBUGGER_FACTORY_ICE_

#include <debugger.ice>

module Ruminate {
	struct DebuggerFactoryOptions {
		string exename;
		long pid;
		long breakpointAddress;
	};

	interface DebuggerFactory {
		Debugger *create( DebuggerFactoryOptions options );
		void shutdown() throws RuminateException;
	};
};

#endif
