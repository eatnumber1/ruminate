from __future__ import print_function
import sys
from RuminateBackend import *
from debugger_impl import *

class DebuggerFactoryImpl(DebuggerFactory):
	def __init__(self, adapter):
		# TODO: Use weakref
		self.debuggers = {}
		self.adapter = adapter

	def shutdown(self, current = None):
		#print("Shutting down")
		for uuid in self.debuggers:
			self.debuggers[uuid].shutdown()
			self.adapter.remove(uuid)
		self.adapter.getCommunicator().shutdown()

	def create(self, options, cbid, current):
		debugee = DebugeePrx.uncheckedCast(current.con.createProxy(cbid))
		debugger = DebuggerImpl(options, self.shutdown, debugee)
		proxy = DebuggerPrx.uncheckedCast(
			current.adapter.addWithUUID(
				debugger
			)
		)
		self.debuggers[proxy.ice_getIdentity()] = debugger
		return proxy
