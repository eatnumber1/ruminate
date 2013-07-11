#ifndef _TYPE_ICE_
#define _TYPE_ICE_

module Ruminate {
	interface Type {
		Type *getPrimitiveType();
		Type *getPointeeType();
		Type *getCanonicalType();

		idempotent string getName();
		idempotent long getSize();

		idempotent int lldbGetTypeClass();
		idempotent int lldbGetNumberOfFields();
		idempotent int lldbGetBasicType();

		Type *lldbGetFieldAtIndex( int index );
		idempotent long lldbGetOffsetInBytes();
		Type *lldbGetType();
	};
};

#endif
