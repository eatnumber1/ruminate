from Ruminate import *
from type_member_impl import *

import lldb

class TypeImpl(Type):
	def __init__(self, sbtype):
		self.sbtype = sbtype
		self.id = {
			lldb.eTypeClassStruct: TypeId.TypeIdStructure
		}[sbtype.type]

	def getName(self, current = None):
		return self.sbtype.GetName()

	def getPrimitiveType(self, current = None):
		return TypeImpl.proxyFor(self.sbtype.GetBasicType(self.sbtype.GetBasicType()), current)

	def getSize(self, current = None):
		return self.sbtype.GetByteSize()

	def lldbGetBasicType(self, current = None):
		return self.sbtype.GetBasicType()

	def getPointeeType(self, current = None):
		return TypeImpl.proxyFor(self.sbtype.GetPointeeType(), current)

	def getPointerType(self, current = None):
		return TypeImpl.proxyFor(self.sbtype.GetPointerType(), current)

	def isComplete(self, current = None):
		return self.sbtype.IsTypeComplete()

	def getCanonicalType(self, current = None):
		canon = self.sbtype.GetCanonicalType()
		if canon == self.sbtype:
			return None
		return TypeImpl.proxyFor(canon, current)

	def getMembers(self, current = None):
		map(lambda f: TypeMemberImpl.proxyFor(f, current), self.sbtype.fields)

	def getArguments(self, current = None):
		atl = self.sbtype.GetFunctionArgumentTypes()
		ret = []
		for idx in range(atl.GetSize()):
			ret.append(TypeImpl.proxyFor(atl.GetTypeAtIndex(idx), current))
		return ret

	def getReturnType(self, current = None):
		return TypeImpl.proxyFor(
			self.sbtype.GetFunctionReturnType(),
			current
		)

	@staticmethod
	def proxyFor(sbtype, current):
		return TypePrx.uncheckedCast(
			current.adapter.addWithUUID(TypeImpl(sbtype))
		)
