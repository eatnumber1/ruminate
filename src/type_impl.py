from Ruminate import *

class TypeImpl(Type):
	def __init__(self, sbtype):
		self.sbtype = sbtype

	def getName(self, current = None):
		return self.sbtype.GetName()

	def getBasicType(self, current = None):
		return self.sbtype.GetBasicType(self.sbtype.GetBasicType())

	def getSize(self, current = None):
		return self.sbtype.GetByteSize()

	def lldbGetTypeClass(self, current = None):
		return self.sbtype.GetTypeClass()

	def lldbGetNumberOfFields(self, current = None):
		return self.sbtype.GetNumberOfFields()

	def lldbGetFieldAtIndex(self, index, current = None):
		return TypePrx.uncheckedCast(current.adapter.addWithUUID(TypeImpl(self.sbtype.GetFieldAtIndex(index))))

	def lldbGetOffsetInBytes(self, current = None):
		return self.sbtype.GetOffsetInBytes();

	def lldbGetType(self, current = None):
		return TypePrx.uncheckedCast(current.adapter.addWithUUID(TypeImpl(self.sbtype.GetType())))
