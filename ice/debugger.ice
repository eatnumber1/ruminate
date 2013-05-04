#ifndef _DEBUGGER_ICE_
#define _DEBUGGER_ICE_

#include <type_inspector.ice>
#include <exceptions.ice>

module Ruminate {
	interface Debugger {
		//TypeInspector *getTypeInspector( long address ) throws RuminateException;
		Type *getTypeByVariableName( string variable, long tid ) throws RuminateException;
	};
};

#endif
