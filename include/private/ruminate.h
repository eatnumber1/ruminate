typedef struct {
	Ice::CommunicatorPtr communicator;
	RuminateBackend::DebuggerFactoryPrx factory;
	RuminateBackend::DebuggerPrx debugger;
	// TODO: Deal with bad states
	Ice::AsyncResultPtr arp;
	GPid child_pid;
} Ruminate;

__attribute__((noinline))
void ruminate_hit_breakpoint() RUMINATE_NOEXCEPT;
