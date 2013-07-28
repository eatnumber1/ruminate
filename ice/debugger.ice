#ifndef _DEBUGGER_ICE_
#define _DEBUGGER_ICE_

#include <exceptions.ice>
#include <type.ice>
#include <frame.ice>

module Ruminate {
	interface Debugger {
		Type *getTypeByVariableName( string variable, long tid ) throws RuminateException;
		FrameList getBacktrace( long tid ) throws RuminateException;
	};
};

#endif
