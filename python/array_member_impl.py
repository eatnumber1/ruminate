from RuminateBackend import *

class ArrayMemberImpl(TypeMember):
	@staticmethod
	def proxyFor(sbtype, base_address, address, type_factory, current):
		return TypeMemberPrx.uncheckedCast(
			current.adapter.addWithUUID(ArrayMemberImpl(sbtype, base_address, address, type_factory))
		)

	def __init__(self, sbtype, base_address, address, type_factory):
		self.type_factory = type_factory
		self.sbtype = sbtype
		self.offset = address - base_address
		self.address = address

	def getType(self, current = None):
		return self.type_factory.proxy(
			sbtype = self.sbtype,
			address = self.address,
			current = current
		)

	def getOffsetInBytes(self, current = None):
		return self.offset
