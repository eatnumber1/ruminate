from Ruminate import *
from type_list_impl import *

class TypeImpl(Type):
	def __init__(self, sbtype):
		self.sbtype = sbtype

	def getName(self, current = None):
		return self.sbtype.GetName()

	def getPrimitiveType(self, current = None):
		return TypeImpl.proxyFor(self.sbtype.GetBasicType(self.sbtype.GetBasicType()), current)

	def getSize(self, current = None):
		return self.sbtype.GetByteSize()

	def lldbGetTypeClass(self, current = None):
		return self.sbtype.GetTypeClass()

	def lldbGetOffsetInBytes(self, current = None):
		return self.sbtype.GetOffsetInBytes();

	def lldbGetOffsetInBits(self, current = None):
		return self.sbtype.GetOffsetInBits();

	def lldbGetBitfieldSizeInBits(self, current = None):
		return self.sbtype.GetBitfieldSizeInBits();

	def lldbIsBitfield(self, current = None):
		return self.sbtype.IsBitfield()

	def lldbGetType(self, current = None):
		return TypeImpl.proxyFor(self.sbtype.GetType(), current)

	def lldbGetBasicType(self, current = None):
		return self.sbtype.GetBasicType()

	def getPointeeType(self, current = None):
		if not self.sbtype.IsPointerType():
			return None
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
		return MemberList.proxyFor(self.sbtype, current)

	def getFunctionArguments(self, current = None):
		if not self.sbtype.IsFunctionType():
			return None
		return FunctionArgumentList.proxyFor(
			self.sbtype.GetFunctionArgumentTypes(),
			current
		)

	def getFunctionReturnType(self, current = None):
		if not self.sbtype.IsFunctionType():
			return None
		return TypeImpl.proxyFor(
			self.sbtype.GetFunctionReturnType(),
			current
		)

	@staticmethod
	def proxyFor(sbtype, current):
		return TypePrx.uncheckedCast(
			current.adapter.addWithUUID(TypeImpl(sbtype))
		)
