from Ruminate import *
from type_member_impl import *

import lldb
import lldb_utils

class TypeImpl(Type):
	def __init__(self, sbtype, sbvalue):
		self.sbvalue = sbvalue
		self.sbtype = sbtype
		if sbtype.type == lldb.eTypeClassBuiltin:
			self.id = {
				#lldb.eBasicTypeBool: TypeId.TypeId,
				lldb.eBasicTypeChar: TypeId.TypeIdChar,
				#lldb.eBasicTypeChar16: TypeId.TypeId,
				#lldb.eBasicTypeChar32: TypeId.TypeId,
				lldb.eBasicTypeDouble: TypeId.TypeIdDouble,
				#lldb.eBasicTypeDoubleComplex: TypeId.TypeId,
				#lldb.eBasicTypeFloat: TypeId.TypeId,
				#lldb.eBasicTypeFloatComplex: TypeId.TypeId,
				#lldb.eBasicTypeHalf: TypeId.TypeId,
				lldb.eBasicTypeInt: TypeId.TypeIdInt,
				#lldb.eBasicTypeInt128: TypeId.TypeId,
				#lldb.eBasicTypeInvalid: TypeId.TypeId,
				lldb.eBasicTypeLong: TypeId.TypeIdLong,
				#lldb.eBasicTypeLongDouble: TypeId.TypeId,
				#lldb.eBasicTypeLongDoubleComplex: TypeId.TypeId,
				#lldb.eBasicTypeLongLong: TypeId.TypeId,
				#lldb.eBasicTypeNullPtr: TypeId.TypeId,
				#lldb.eBasicTypeObjCClass: TypeId.TypeId,
				#lldb.eBasicTypeObjCID: TypeId.TypeId,
				#lldb.eBasicTypeObjCSel: TypeId.TypeId,
				lldb.eBasicTypeShort: TypeId.TypeIdShort,
				lldb.eBasicTypeSignedChar: TypeId.TypeIdChar,
				#lldb.eBasicTypeSignedWChar: TypeId.TypeId,
				lldb.eBasicTypeUnsignedChar: TypeId.TypeIdChar,
				lldb.eBasicTypeUnsignedInt: TypeId.TypeIdInt,
				#lldb.eBasicTypeUnsignedInt128: TypeId.TypeId,
				lldb.eBasicTypeUnsignedLong: TypeId.TypeIdLong,
				#lldb.eBasicTypeUnsignedLongLong: TypeId.TypeId,
				lldb.eBasicTypeUnsignedShort: TypeId.TypeIdShort,
				#lldb.eBasicTypeUnsignedWChar: TypeId.TypeId,
				lldb.eBasicTypeVoid: TypeId.TypeIdVoid,
				#lldb.eBasicTypeWChar: TypeId.TypeId,
			}[sbtype.GetBasicType()]
		else:
			self.id = {
				lldb.eTypeClassArray: TypeId.TypeIdArray,
				#lldb.eTypeClassBlockPointer: TypeId.,
				#lldb.eTypeClassClass: TypeId.,
				#lldb.eTypeClassComplexFloat: TypeId.,
				#lldb.eTypeClassComplexInteger: TypeId.,
				#lldb.eTypeClassEnumeration: TypeId.,
				#lldb.eTypeClassMemberPointer: TypeId.,
				#lldb.eTypeClassObjCInterface: TypeId.,
				#lldb.eTypeClassObjCObject: TypeId.,
				#lldb.eTypeClassObjCObjectPointer: TypeId.,
				#lldb.eTypeClassReference: TypeId.,
				#lldb.eTypeClassUnion: TypeId.,
				#lldb.eTypeClassVector: TypeId.,
				lldb.eTypeClassStruct: TypeId.TypeIdStructure,
				lldb.eTypeClassFunction: TypeId.TypeIdFunction,
				lldb.eTypeClassPointer: TypeId.TypeIdPointer,
				lldb.eTypeClassTypedef: TypeId.TypeIdTypedef,
				lldb.eTypeClassInvalid: TypeId.TypeIdUnknown,
				lldb.eTypeClassOther: TypeId.TypeIdUnknown
			}[sbtype.type]

	def getId(self, current = None):
		return self.id

	def getName(self, current = None):
		return self.sbtype.GetName()

	def getBuiltinType(self, current = None):
		return TypeImpl.proxyFor(current, self.sbtype.GetBasicType(self.sbtype.GetBasicType()), self.sbvalue)

	def getSize(self, current = None):
		return self.sbtype.GetByteSize()

	def getPointeeType(self, current = None):
		return TypeImpl.proxyFor(self.sbtype.GetPointeeType(), self.sbvalue.Dereference(), current)

	def getPointerType(self, current = None):
		return TypeImpl.proxyFor(self.sbtype.GetPointerType(), self.sbvalue.AddressOf(), current)

	def isComplete(self, current = None):
		return self.sbtype.IsTypeComplete()

	def getCanonicalType(self, current = None):
		canon = self.sbtype.GetCanonicalType()
		if canon == self.sbtype:
			return None
		return TypeImpl.proxyFor(canon, self.sbvalue, current)

	def getMembers(self, current = None):
		ret = []
		for field in self.sbtype.fields:
			ret.append(
				TypeMemberImpl.proxyFor(
					field,
					self.sbvalue.GetChildMemberWithName(field.name),
					current
				)
			)
		return ret

	def getArguments(self, current = None):
		atl = self.sbtype.GetFunctionArgumentTypes()
		ret = []
		for idx in range(atl.GetSize()):
			ret.append(TypeImpl.proxyFor(current, atl.GetTypeAtIndex(idx), None))
		return ret

	def getReturnType(self, current = None):
		return TypeImpl.proxyFor(
			current,
			self.sbtype.GetFunctionReturnType(),
			None,
		)

	def isSigned(self, current = None):
		# TODO: Check that this is a builtin type
		return {
			#lldb.eBasicTypeBool: ,
			lldb.eBasicTypeChar: False,
			#lldb.eBasicTypeChar16: ,
			#lldb.eBasicTypeChar32: ,
			lldb.eBasicTypeDouble: False,
			#lldb.eBasicTypeDoubleComplex: ,
			#lldb.eBasicTypeFloat: ,
			#lldb.eBasicTypeFloatComplex: ,
			#lldb.eBasicTypeHalf: ,
			lldb.eBasicTypeInt: False,
			#lldb.eBasicTypeInt128: ,
			#lldb.eBasicTypeInvalid: ,
			lldb.eBasicTypeLong: False,
			#lldb.eBasicTypeLongDouble: ,
			#lldb.eBasicTypeLongDoubleComplex: ,
			#lldb.eBasicTypeLongLong: ,
			#lldb.eBasicTypeNullPtr: ,
			#lldb.eBasicTypeObjCClass: ,
			#lldb.eBasicTypeObjCID: ,
			#lldb.eBasicTypeObjCSel: ,
			lldb.eBasicTypeShort: False,
			lldb.eBasicTypeSignedChar: True,
			#lldb.eBasicTypeSignedWChar: ,
			lldb.eBasicTypeUnsignedChar: False,
			lldb.eBasicTypeUnsignedInt: False,
			#lldb.eBasicTypeUnsignedInt128: ,
			lldb.eBasicTypeUnsignedLong: False,
			#lldb.eBasicTypeUnsignedLongLong: ,
			lldb.eBasicTypeUnsignedShort: False,
			#lldb.eBasicTypeUnsignedWChar: ,
			lldb.eBasicTypeVoid: False,
			#lldb.eBasicTypeWChar: ,
		}[self.sbtype.GetBasicType()]

	def isUnsigned(self, current = None):
		# TODO: Check that this is a builtin type
		return {
			#lldb.eBasicTypeBool: ,
			lldb.eBasicTypeChar: False,
			#lldb.eBasicTypeChar16: ,
			#lldb.eBasicTypeChar32: ,
			lldb.eBasicTypeDouble: False,
			#lldb.eBasicTypeDoubleComplex: ,
			#lldb.eBasicTypeFloat: ,
			#lldb.eBasicTypeFloatComplex: ,
			#lldb.eBasicTypeHalf: ,
			lldb.eBasicTypeInt: False,
			#lldb.eBasicTypeInt128: ,
			#lldb.eBasicTypeInvalid: ,
			lldb.eBasicTypeLong: False,
			#lldb.eBasicTypeLongDouble: ,
			#lldb.eBasicTypeLongDoubleComplex: ,
			#lldb.eBasicTypeLongLong: ,
			#lldb.eBasicTypeNullPtr: ,
			#lldb.eBasicTypeObjCClass: ,
			#lldb.eBasicTypeObjCID: ,
			#lldb.eBasicTypeObjCSel: ,
			lldb.eBasicTypeShort: False,
			lldb.eBasicTypeSignedChar: False,
			#lldb.eBasicTypeSignedWChar: ,
			lldb.eBasicTypeUnsignedChar: True,
			lldb.eBasicTypeUnsignedInt: True,
			#lldb.eBasicTypeUnsignedInt128: ,
			lldb.eBasicTypeUnsignedLong: True,
			#lldb.eBasicTypeUnsignedLongLong: ,
			lldb.eBasicTypeUnsignedShort: True,
			#lldb.eBasicTypeUnsignedWChar: ,
			lldb.eBasicTypeVoid: False,
			#lldb.eBasicTypeWChar: ,
		}[self.sbtype.GetBasicType()]

	@staticmethod
	def proxyFor(sbtype, sbvalue, current):
		return TypePrx.uncheckedCast(
			current.adapter.addWithUUID(TypeImpl(sbtype, sbvalue))
		)
