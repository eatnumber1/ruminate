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
		Type *getPointeeType( long address );
		Type *getPointerType( long address );
		Type *getCanonicalType();
		Type *getReturnType();

		TypeMemberList getMembers();
		TypeList getArguments();

		string getName(); // This is cached in the C++ API layer
		idempotent long getSize();

		idempotent bool isComplete();
		idempotent bool isSigned();
		idempotent bool isUnsigned();
	};
};

#endif
