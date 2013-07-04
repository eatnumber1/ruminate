#include "ruminate/rdebugger.h"

#include <Ice/Ice.h>
#include "ice/debugger_factory.h"

Ruminate::DebuggerFactoryPrx &rdebugger_get_factory( RDebugger *debugger );
