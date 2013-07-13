#ifndef _TYPE_ICE_
#define _TYPE_ICE_

#include <type_list.ice>

module Ruminate {
	interface Type {
		Type *getPrimitiveType();
		Type *getPointeeType();
		Type *getCanonicalType();
		Type *getFunctionReturnType();

		TypeList *getStructFields();
		TypeList *getFunctionArguments();

		idempotent string getName();
		idempotent long getSize();

		idempotent int lldbGetTypeClass();
		idempotent int lldbGetBasicType();

		idempotent long lldbGetOffsetInBytes();
		Type *lldbGetType();
	};
};

#endif
