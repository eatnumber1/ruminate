#ifndef _TYPE_LIST_ICE_
#define _TYPE_LIST_ICE_

module Ruminate {
	interface Type;

	interface TypeList {
		Type *getTypeAtIndex( int index );
		int getLength();
	};
};

#endif
