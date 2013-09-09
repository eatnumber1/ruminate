from Ruminate import *

class TypeMemberImpl(TypeMember):
	@staticmethod
	def proxyFor(sbtypemember, base_address, thread_stop, current):
		return TypeMemberPrx.uncheckedCast(
			current.adapter.addWithUUID(TypeMemberImpl(sbtypemember, base_address, thread_stop))
		)

	def __init__(self, sbtypemember, base_address, thread_stop):
		self.sbtypemember = sbtypemember
		self.thread_stop = thread_stop
		self.address = base_address + self.getOffsetInBytes()

	def getType(self, current = None):
		from type_impl import TypeImpl
		return TypeImpl.proxyFor(self.sbtypemember.type, address, self.thread_stop, current)

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
