from type_impl import *

class TypeImplFactory(object):
	def instance(self, **kwargs):
		return TypeImpl(
			kwargs["sbtype"] if "sbtype" in kwargs else self.sbtype,
			kwargs["thread_stop"] if "thread_stop" in kwargs else self.thread_stop,
			kwargs["debugger"] if "debugger" in kwargs else self.debugger
		)

	def proxy(self, **kwargs):
		return TypePrx.uncheckedCast(
			kwargs["current"].adapter.addWithUUID(self.instance(**kwargs))
		)
