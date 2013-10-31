from RuminateBackend import *

class SBTypeListAdapter(TypeMember):
	@staticmethod
	def proxyFor(**kwargs):
		current = kwargs.pop("current")
		return TypeMemberPrx.uncheckedCast(
			current.adapter.addWithUUID(
				SBTypeListAdapter(**kwargs)
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

class SBTypeEnumMemberAdapter(TypeMember):
	@staticmethod
	def proxyFor(**kwargs):
		current = kwargs.pop("current")
		return TypeMemberPrx.uncheckedCast(
			current.adapter.addWithUUID(SBTypeEnumMemberAdapter(**kwargs))
		)

	def __init__(self, sbtypemember, base_address, type_factory):
		self.sbtypemember = sbtypemember
		self.type_factory = type_factory
		self.address = base_address

	def getType(self, current = None):
		return self.type_factory.proxy(
			sbtype = self.sbtypemember.type,
			address = self.address,
			current = current
		)

	def getName(self, current = None):
		return self.sbtypemember.name

	def getOffsetInBytes(self, current = None):
		return 0

	def getValueSigned(self, current = None):
		return self.sbtypemember.signed

class SBTypeMemberAdapter(TypeMember):
	@staticmethod
	def proxyFor(**kwargs):
		current = kwargs.pop("current")
		return TypeMemberPrx.uncheckedCast(
			current.adapter.addWithUUID(SBTypeMemberAdapter(**kwargs))
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

class SBTypeAdapter(TypeMember):
	@staticmethod
	def proxyFor(**kwargs):
		current = kwargs.pop("current")
		return TypeMemberPrx.uncheckedCast(
			current.adapter.addWithUUID(SBTypeAdapter(**kwargs))
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
