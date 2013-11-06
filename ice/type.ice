#ifndef _TYPE_ICE_
#define _TYPE_ICE_

#include <type_member.ice>

module RuminateBackend {
	enum TypeId {
		TypeIdStructure,
		TypeIdEnum,
		TypeIdUnion,
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
		TypeIdBool,

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
		idempotent long getArrayLength();

		TypeMemberList getMembers( optional(1) long tid );

		string getName(); // This is cached in the C++ API layer
		idempotent long getSize();

		idempotent bool isComplete();
		idempotent bool isSigned();
		idempotent bool isUnsigned();
	};
};

#endif
