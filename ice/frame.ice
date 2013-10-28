#ifndef _STACKTRACE_ICE_
#define _STACKTRACE_ICE_

#include <type.ice>

module RuminateBackend {
	struct Frame {
		string functionName;
		string moduleName;
		string compileUnitName;
		Type *functionType;
		long line;
	};
	sequence<Frame> FrameList;
};

#endif
