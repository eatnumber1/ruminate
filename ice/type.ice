#ifndef _TYPE_ICE_
#define _TYPE_ICE_

module Ruminate {
	interface Type {
		Type *getBasicType();

		idempotent string getName();
		idempotent long getSize();

		idempotent int lldbGetTypeClass();
		idempotent int lldbGetNumberOfFields();

		Type *lldbGetFieldAtIndex( int index );
		idempotent long lldbGetOffsetInBytes();
		Type *lldbGetType();
	};
};

#endif
