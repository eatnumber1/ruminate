#include "ruminate/init.h"

#include <lldb/API/LLDB.h>

void ruminate_initialize() {
	lldb::SBDebugger::Initialize();
}

void ruminate_terminate() {
	lldb::SBDebugger::Terminate();
}
