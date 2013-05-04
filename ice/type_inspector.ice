#ifndef _TYPE_INSPECTOR_ICE_
#define _TYPE_INSPECTOR_ICE_

#include <type.ice>
#include <exceptions.ice>

module Ruminate {
	interface TypeInspector {
		long getBreakpointAddress();

		Type *getTypeByVariableName( string variable, long tid ) throws RuminateException;
	};
};

#endif
