#ifndef _TYPE_MEMBER_ICE_
#define _TYPE_MEMBER_ICE_

module RuminateBackend {
	interface Type;

	// TODO: Subclass TypeMember from some shared superclass for arrays
	interface TypeMember {
		idempotent Type *getType();

		string getName(); // This is cached in the C++ API layer

		idempotent long getOffsetInBytes();
		idempotent long getOffsetInBits();

		idempotent long getBitfieldSizeInBits();

		idempotent bool isBitfield();

		// TODO: Properly support unsigned values
		idempotent long getValueSigned();
	};

	sequence<TypeMember *> TypeMemberList;
};

#endif
