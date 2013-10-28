typedef struct {
	Ice::CommunicatorPtr communicator;
	RuminateBackend::DebuggerFactoryPrx factory;
	RuminateBackend::DebuggerPrx debugger;
	// TODO: Deal with bad states
	Ice::AsyncResultPtr arp;
	GPid child_pid;
} Ruminate;
