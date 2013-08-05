from Ruminate import *

class TypeListImpl(TypeList):
	@staticmethod
	def proxyFor(sbtypelist, current):
		return TypeListPrx.uncheckedCast(
			current.adapter.addWithUUID(sbtypelist)
		)

class FunctionArgumentList(TypeListImpl):
	def __init__(self, tlist):
		self.tlist = tlist

	def getLength(self, current = None):
		return self.tlist.GetSize()

	def getTypeAtIndex(self, index, current = None):
		from type_impl import TypeImpl
		return TypeImpl.proxyFor(self.tlist.GetTypeAtIndex(index), current)

	@staticmethod
	def proxyFor(tlist, current):
		return TypeListImpl.proxyFor(FunctionArgumentList(tlist), current)

class MemberList(TypeListImpl):
	def __init__(self, struct):
		self.struct = struct

	def getLength(self, current = None):
		return self.struct.GetNumberOfFields()

	def getTypeAtIndex(self, index, current = None):
		from type_impl import TypeImpl
		return TypeImpl.proxyFor(self.struct.GetFieldAtIndex(index), current)

	@staticmethod
	def proxyFor(struct, current):
		return TypeListImpl.proxyFor(MemberList(struct), current)
