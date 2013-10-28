from __future__ import nested_scopes, with_statement

from RuminateBackend import *
from array_member_impl import *
from stopped_thread import *
import type_member_impl

import lldb
import lldb_utils

class TypeImpl(Type):
	def __init__(self, sbtype, address, thread_stop, debugger):
		from type_impl_factory import TypeImplFactory
		self.factory = TypeImplFactory()
		self.factory.sbtype = sbtype
		self.factory.address = address
		self.factory.thread_stop = thread_stop
		self.factory.debugger = debugger

		self.debugger = debugger
		self.address = address
		self.sbtype = sbtype
		self.thread_stop = thread_stop
		if sbtype.type == lldb.eTypeClassBuiltin:
			self.id = {
				lldb.eBasicTypeBool: TypeId.TypeIdBool,
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
				lldb.eTypeClassEnumeration: TypeId.TypeIdEnum,
				#lldb.eTypeClassMemberPointer: TypeId.,
				#lldb.eTypeClassObjCInterface: TypeId.,
				#lldb.eTypeClassObjCObject: TypeId.,
				#lldb.eTypeClassObjCObjectPointer: TypeId.,
				#lldb.eTypeClassReference: TypeId.,
				lldb.eTypeClassUnion: TypeId.TypeIdUnion,
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

	def getName(self, tid, current = None):
		with self.thread_stop.produce(tid):
			if self.id == TypeId.TypeIdFunction:
				return self.debugger.createSBAddressFor(self.address).function.name
			else:
				return self.sbtype.name

	def getBuiltinType(self, current = None):
		return self.factory.proxy(sbtype = self.sbtype.GetBasicType(self.sbtype.GetBasicType()), current = current)

	def getSize(self, current = None):
		return self.sbtype.GetByteSize()

	def getPointeeType(self, address, current = None):
		return self.factory.proxy(sbtype = self.sbtype.GetPointeeType(), address = address, current = current)

	def getPointerType(self, address, current = None):
		return self.factory.proxy(sbtype = self.sbtype.GetPointerType(), address = address, current = current)

	def isComplete(self, current = None):
		return self.sbtype.IsTypeComplete()

	def getCanonicalType(self, current = None):
		canon = self.sbtype.GetCanonicalType()
		if canon == self.sbtype:
			return None
		return self.factory.proxy(sbtype = canon, current = current)

	def getMembers(self, tid, current = None):
		with self.thread_stop.produce(tid):
			if self.id == TypeId.TypeIdArray:
				ret = []
				array = self.debugger.createSBValueFor(self.sbtype, self.address)
				for index in range(0, array.num_children):
					child = array.GetChildAtIndex(index, lldb.eDynamicDontRunTarget, False)
					ret.append(
						type_member_impl.SBTypeAdapter.proxyFor(
							sbtype = child.type,
							base_address = self.address,
							address = child.address_of.unsigned,
							type_factory = self.factory,
							current = current
						)
					)
				return ret
			elif self.id == TypeId.TypeIdFunction:
				ret = []
				args = self.sbtype.GetFunctionArgumentTypes()
				for index in range(0, args.GetSize()):
					ret.append(
						type_member_impl.SBTypeListAdapter.proxyFor(
							sbtypelist = args,
							index = index,
							type_factory = self.factory,
							current = current
						)
					)
				return ret
			elif self.id == TypeId.TypeIdEnum:
				ret = []
				for enum_member in self.sbtype.enum_members:
					ret.append(
						type_member_impl.SBTypeMemberAdapter.proxyFor(
							sbtypemember = enum_member,
							base_address = self.address,
							type_factory = self.factory,
							current = current
						)
					)
			else:
				ret = []
				for field in self.sbtype.fields:
					ret.append(
						type_member_impl.SBTypeMemberAdapter.proxyFor(
							sbtypemember = field,
							base_address = self.address,
							type_factory = self.factory,
							current = current
						)
					)
				return ret

	def getArguments(self, current = None):
		atl = self.sbtype.GetFunctionArgumentTypes()
		ret = []
		for idx in range(atl.GetSize()):
			ret.append(
				self.factory.proxy(
					sbtype = atl.GetTypeAtIndex(idx),
					current = current,
					address = None
				)
			)
		return ret

	def getReturnType(self, current = None):
		return self.factory.proxy(
			current = current,
			sbtype = self.sbtype.GetFunctionReturnType(),
			address = None,
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
