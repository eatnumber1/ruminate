from Ruminate import *

class TypeImpl(Type):
	def __init__(self, sbtype):
		self.sbtype = sbtype

	def getName(self, current = None):
		return self.sbtype.GetName()

	def getPrimitiveType(self, current = None):
		return self._typeProxy(self.sbtype.GetBasicType(self.sbtype.GetBasicType()), current)

	def getSize(self, current = None):
		return self.sbtype.GetByteSize()

	def lldbGetTypeClass(self, current = None):
		return self.sbtype.GetTypeClass()

	def lldbGetNumberOfFields(self, current = None):
		return self.sbtype.GetNumberOfFields()

	def lldbGetFieldAtIndex(self, index, current = None):
		return self._typeProxy(self.sbtype.GetFieldAtIndex(index), current)

	def lldbGetOffsetInBytes(self, current = None):
		return self.sbtype.GetOffsetInBytes();

	def lldbGetType(self, current = None):
		return self._typeProxy(self.sbtype.GetType(), current)

	def lldbGetBasicType(self, current = None):
		return self.sbtype.GetBasicType()

	def getPointeeType(self, current = None):
		if not self.sbtype.IsPointerType():
			return None
		return self._typeProxy(self.sbtype.GetPointerType(), current)

	def getCanonicalType(self, current = None):
		canon = self.sbtype.GetCanonicalType()
		if canon == self.sbtype:
			return None
		return self._typeProxy(canon, current)

	def _typeProxy(self, sbtype, current):
		return TypePrx.uncheckedCast(current.adapter.addWithUUID(TypeImpl(sbtype)))
