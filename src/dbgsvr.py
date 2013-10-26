#!/usr/bin/env python
import sys, os, struct, Ice
from Ruminate import *
from debugger_factory_impl import *

try:
	import lldb
except ImportError:
	sys.path.insert(0, subprocess.check_output("lldb -P", shell = True).strip())
	import lldb

class Server(Ice.Application):
	def interruptCallback(self, sig):
		if self.factory != None:
			del self.factory

	def run(self, args):
		self.shutdownOnInterrupt()
		adapter = self.communicator().createObjectAdapterWithEndpoints("RuminateAdapter", "default -h 127.0.0.1")
		self.factory = DebuggerFactoryImpl(adapter)

		adapter.add(self.factory, Ice.Identity("DebuggerFactory"))

		adapter.activate()

		# Signal that we're ready and tell the parent our port
		packed_port = struct.pack("i", adapter.getEndpoints()[0].getInfo().port)
		sys.stdout.write(packed_port)

		sys.stdout.close()
		os.close(1)
		sys.stdout = sys.stderr
		os.dup2(2, 1)

		self.communicator().waitForShutdown()
		return 0

sys.exit(Server().main(sys.argv))
