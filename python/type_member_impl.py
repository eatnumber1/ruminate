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

	def __init__(self, sbtypemember, type_factory):
		self.sbtypemember = sbtypemember
		self.type_factory = type_factory

	def getType(self, current = None):
		return self.type_factory.proxy(
			sbtype = self.sbtypemember.type,
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

	def __init__(self, sbtypemember, type_factory):
		self.sbtypemember = sbtypemember
		self.type_factory = type_factory

	def getType(self, current = None):
		return self.type_factory.proxy(
			sbtype = self.sbtypemember.type,
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

	def __init__(self, sbtype, type_factory, offset):
		self.type_factory = type_factory
		self.sbtype = sbtype
		self.offset = offset

	def getType(self, current = None):
		return self.type_factory.proxy(
			sbtype = self.sbtype,
			current = current
		)

	def getOffsetInBytes(self, current = None):
		return self.offset
