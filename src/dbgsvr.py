#!/usr/bin/env python
import sys, os, signal, Ice
from Ruminate import *
from debugger_factory_impl import *

class Server(Ice.Application):
	def interruptCallback(self, sig):
		if self.factory != None:
			del self.factory

	def run(self, args):
		self.shutdownOnInterrupt()
		adapter = self.communicator().createObjectAdapterWithEndpoints("RuminateAdapter", "default -h 127.0.0.1")
		self.factory = DebuggerFactoryImpl(adapter)

		adapter.add(self.factory, Ice.Identity("DebuggerFactory -h 127.0.0.1 -p 1024"))

		adapter.activate()

		# Signal that we're ready
		os.kill(int(sys.argv[1]), signal.SIGUSR1)

		self.communicator().waitForShutdown()
		return 0

sys.exit(Server().main(sys.argv))
