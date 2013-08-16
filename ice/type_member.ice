#ifndef _TYPE_MEMBER_ICE_
#define _TYPE_MEMBER_ICE_

module Ruminate {
	interface Type;

	interface TypeMember {
		idempotent Type *getType();

		string getName(); // This is cached in the C++ API layer

		idempotent long getOffsetInBytes();
		idempotent long getOffsetInBits();

		idempotent long getBitfieldSizeInBits();

		idempotent bool isBitfield();
	};

	sequence<TypeMember *> TypeMemberList;
};

#endif
