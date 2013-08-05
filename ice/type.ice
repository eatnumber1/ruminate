#ifndef _TYPE_ICE_
#define _TYPE_ICE_

#include <type_list.ice>

module Ruminate {
	interface Type {
		Type *getPrimitiveType();
		Type *getPointeeType();
		Type *getPointerType();
		Type *getCanonicalType();
		Type *getFunctionReturnType();

		TypeList *getMembers();
		TypeList *getFunctionArguments();

		string getName(); // This is cached in the C++ API layer
		idempotent long getSize();

		idempotent bool isComplete();

		idempotent int lldbGetTypeClass();
		idempotent int lldbGetBasicType();

		idempotent long lldbGetOffsetInBytes();
		idempotent long lldbGetOffsetInBits();

		idempotent long lldbGetBitfieldSizeInBits();

		idempotent bool lldbIsBitfield();

		Type *lldbGetType();
	};
};

#endif
