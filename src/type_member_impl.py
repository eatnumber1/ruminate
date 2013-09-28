from Ruminate import *

class TypeListMemberImpl(TypeMember):
	@staticmethod
	def proxyFor(sbtypelist, index, type_factory, current):
		return TypeMemberPrx.uncheckedCast(
			current.adapter.addWithUUID(
				TypeListMemberImpl(sbtypelist, index, type_factory)
			)
		)

	def __init__(self, sbtypelist, index, type_factory):
		self.sbtypelist = sbtypelist
		self.index = index
		self.type_factory = type_factory

	def getType(self, current = None):
		return self.type_factory.proxy(
			sbtype = self.sbtypelist.GetTypeAtIndex(self.index),
			current = current
		)

class TypeMemberImpl(TypeMember):
	@staticmethod
	def proxyFor(sbtypemember, base_address, type_factory, current):
		return TypeMemberPrx.uncheckedCast(
			current.adapter.addWithUUID(TypeMemberImpl(sbtypemember, base_address, type_factory))
		)

	def __init__(self, sbtypemember, base_address, type_factory):
		self.sbtypemember = sbtypemember
		self.type_factory = type_factory
		self.address = base_address + self.getOffsetInBytes()

	def getType(self, current = None):
		return self.type_factory.proxy(
			sbtype = self.sbtypemember.type,
			address = self.address,
			current = current
		)

	def getName(self, current = None):
		return self.sbtypemember.name

	def getOffsetInBytes(self, current = None):
		return self.sbtypemember.byte_offset

	def getOffsetInBits(self, current = None):
		return self.sbtypemember.bit_offset

	def getBitfieldSizeInBits(self, current = None):
		return self.sbtypemember.bitfield_bit_size

	def isBitfield(self, current = None):
		return self.sbtypemember.is_bitfield
