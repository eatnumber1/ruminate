#ifndef _TYPE_ICE_
#define _TYPE_ICE_

#include <type_member.ice>

module Ruminate {
	enum TypeId {
		TypeIdStructure
	};

	interface Type;
	sequence<Type *> TypeList;

	interface Type {
		TypeId getId();

		Type *getPrimitiveType();
		Type *getPointeeType();
		Type *getPointerType();
		Type *getCanonicalType();
		Type *getReturnType();

		TypeMemberList getMembers();
		TypeList getArguments();

		string getName(); // This is cached in the C++ API layer
		idempotent long getSize();

		idempotent bool isComplete();

		idempotent int lldbGetTypeClass();
		idempotent int lldbGetBasicType();

		Type *lldbGetType();
	};
};

#endif
