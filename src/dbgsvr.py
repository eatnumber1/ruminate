#!/usr/bin/env python
import sys, Ice
from Ruminate import *
from debugger_factory_impl import *

class Server(Ice.Application):
	def interruptCallback(self, sig):
		if self.factory != None:
			del self.factory

	def run(self, args):
		self.shutdownOnInterrupt()
		adapter = self.communicator().createObjectAdapterWithEndpoints("RuminateAdapter", "default -p 1024")
		self.factory = DebuggerFactoryImpl(adapter)

		adapter.add(self.factory, Ice.Identity("DebuggerFactory"))

		adapter.activate()
		self.communicator().waitForShutdown()
		return 0

sys.exit(Server().main(sys.argv))
