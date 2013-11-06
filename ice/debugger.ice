#ifndef _DEBUGGER_ICE_
#define _DEBUGGER_ICE_

#include <exceptions.ice>
#include <type.ice>
#include <frame.ice>

module RuminateBackend {
	interface Debugger {
		Type *getTypeByVariableName( string variable, long tid ) throws RuminateException;
		Type *getTypeByName( string name, optional(1) string modname, optional(2) string cuname ) throws RuminateException;
		FrameList getBacktrace( long tid ) throws RuminateException;
		string getFunctionName( long addr ) throws RuminateException;
	};
};

#endif
