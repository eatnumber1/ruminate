typedef struct {
	Ice::CommunicatorPtr communicator;
	Ruminate::DebuggerFactoryPrx factory;
	Ruminate::DebuggerPrx debugger;
	// TODO: Deal with bad states
	Ice::AsyncResultPtr arp;
	GPid child_pid;
} Rumination;

#if 0
#ifndef _RUMINATE_CPP_
extern template gxx_call_proto(Ice::CommunicatorPtr);
extern template gxx_call_proto(Ice::ObjectPrx);
extern template gxx_call_proto(Ice::AsyncResultPtr);
extern template gxx_call_proto(Ruminate::DebuggerFactoryPrx);
extern template gxx_call_proto(Ruminate::DebuggerPrx);
#endif
#endif
