typedef struct {
	Ice::CommunicatorPtr communicator;
	Ruminate::DebuggerFactoryPrx factory;
	Ruminate::DebuggerPrx debugger;
	// TODO: Deal with bad states
	Ice::AsyncResultPtr arp;
	GPid child_pid;
} Rumination;
