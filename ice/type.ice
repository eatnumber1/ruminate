#ifndef _TYPE_ICE_
#define _TYPE_ICE_

#include <type_member.ice>

module Ruminate {
	enum TypeId {
		TypeIdStructure,
		TypeIdFunction,
		TypeIdTypedef,
		TypeIdPointer,
		TypeIdArray,

		TypeIdInt,
		TypeIdLong,
		TypeIdDouble,
		TypeIdShort,
		TypeIdChar,
		TypeIdVoid,

		TypeIdUnknown
	};

	interface Type;
	sequence<Type *> TypeList;

	interface Type {
		TypeId getId();

		Type *getBuiltinType();
		Type *getPointeeType();
		Type *getPointerType();
		Type *getCanonicalType();
		Type *getReturnType();
		Type *getArrayMemberType();

		TypeMemberList getMembers();
		TypeList getArguments();

		string getName(); // This is cached in the C++ API layer
		idempotent long getSize();
		idempotent long getArraySize();

		idempotent bool isComplete();
		idempotent bool isSigned();
		idempotent bool isUnsigned();
	};
};

#endif
