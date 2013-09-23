from type_impl import *

class TypeImplFactory(object):
	def instance(self, sbtype = self.sbtype, address = self.address, debugger = self.debugger, thread_stop = self.thread_stop):
		TypeImpl(self.sbtype, self.address, self.thread_stop, self.debugger)

	def proxy(self, sbtype = self.sbtype, address = self.address, debugger = self.debugger, thread_stop = self.thread_stop, current = self.current):
		return TypePrx.uncheckedCast(
			current.adapter.addWithUUID(self.instance(sbtype, address, debugger, thread_stop))
		)
