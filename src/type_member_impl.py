from Ruminate import *

class TypeMemberImpl(TypeMember):
	@staticmethod
	def proxyFor(sbtypemember, current):
		return TypeMemberPrx.uncheckedCast(
			current.adapter.addWithUUID(TypeMemberImpl(sbtypemember))
		)

	def __init__(self, sbtypemember):
		self.sbtypemember = sbtypemember

	def getType(self, current = None):
		from type_impl import TypeImpl
		return TypeImpl.proxyFor(self.sbtypemember.type, current)

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
