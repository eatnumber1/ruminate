#include "ruminate/init.h"

#include "private/lldb/sbdebugger.h"

void ruminate_initialize() {
	sbdebugger_initialize();
}

void ruminate_terminate() {
	sbdebugger_terminate();
}
