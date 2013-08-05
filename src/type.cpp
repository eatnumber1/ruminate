#include <sys/types.h>
#include <stdint.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include <exception>

#include <glib.h>

#include <Ice/Ice.h>
#include "ice/type.h"
#include "ice/type_list.h"

#include "ruminate/errors.h"
#include "ruminate/type.h"

#include "private/type.h"

struct RType {
	char *name;
	gint refcnt;
	RTypeIdentifier id;
	Ruminate::TypePrx proxy;

	RType() :
		name(NULL),
		refcnt(1),
		id(R_TYPE_CLASS_INVALID),
		proxy(0)
	{}
};

typedef struct RContainerType {
	RType type;
	Ruminate::TypeListPrx memberlist;

	RContainerType() :
		type(),
		memberlist(0)
	{}
} RContainerType;

typedef struct RMember {
	RType *type;
	gint refcnt;
	Ruminate::TypePrx proxy;
	char *name;

	RMember() :
		type(NULL),
		refcnt(1),
		proxy(0),
		name(NULL)
	{}
} RMember;

struct RStructMember {
	RMember member;

	RStructMember() :
		member()
	{}
};

struct RBitFieldMember {
	RMember member;

	RBitFieldMember() :
		member()
	{}
};

#if 0
struct RStructType {
	RType type;
	Ruminate::TypeListPrx memberlist;

	RStructType() :
		type(),
		memberlist(0)
	{}
};
#endif

struct RStructType {
	RContainerType container_type;

	RStructType() :
		container_type()
	{}
};

struct RBitFieldType {
	RContainerType container_type;

	RBitFieldType() :
		container_type()
	{}
};

struct RPrimitiveType {
	RType type;
	RPrimitiveTypeIdentifier id;

	RPrimitiveType() :
		type(),
		id(R_PRIMITIVE_TYPE_INVALID)
	{}
};

struct RFunctionType {
	RType type;
	Ruminate::TypeListPrx arglist;

	RFunctionType() :
		type(),
		arglist(0)
	{}
};

G_BEGIN_DECLS

void r_type_unref( RType *type ) {
	if( g_atomic_int_dec_and_test(&type->refcnt) ) {
		if( type->name != NULL ) g_free(type->name);

		if( type->proxy->lldbIsBitfield() ) {
			RBitFieldType *bf = (RBitFieldType *) type;
			bf->~RBitFieldType();
			g_slice_free(RBitFieldType, bf);
		} else {
			switch( type->id ) {
				case R_TYPE_CLASS_STRUCT: {
					RStructType *st = (RStructType *) type;
					st->~RStructType();
					g_slice_free(RStructType, st);
					break;
				}
				case R_TYPE_CLASS_PRIMITIVE: {
					RPrimitiveType *pt = (RPrimitiveType *) type;
					pt->~RPrimitiveType();
					g_slice_free(RPrimitiveType, pt);
					break;
				}
				case R_TYPE_CLASS_FUNCTION: {
					RFunctionType *ft = (RFunctionType *) type;
					ft->~RFunctionType();
					g_slice_free(RFunctionType, ft);
				}
				default:
					type->~RType();
					g_slice_free(RType, type);
			}
		}
	}
}

void r_type_ref( RType *type ) {
	g_atomic_int_inc(&type->refcnt);
}

RType *r_type_new( Ruminate::TypePrx proxy, GError **err ) {
	static_assert(sizeof(RTypeIdentifier) <= sizeof(uint32_t), "An RTypeIdentifier can't fit in a uint32_t!");
	RTypeIdentifier id = (RTypeIdentifier) proxy->lldbGetTypeClass();

	RType *ret = NULL;
	if( proxy->lldbIsBitfield() ) {
		RBitFieldType *bf = g_slice_new(RBitFieldType);
		new (bf) RBitFieldType();
		ret = (RType *) bf;
	} else {
		switch( id ) {
			case R_TYPE_CLASS_STRUCT: {
				RStructType *st = g_slice_new(RStructType);
				new (st) RStructType();
				ret = (RType *) st;
				break;
			}
			case R_TYPE_CLASS_PRIMITIVE:
				ret = (RType *) g_slice_new(RPrimitiveType);
				new (ret) RPrimitiveType();
				break;
			case R_TYPE_CLASS_FUNCTION: {
				RFunctionType *ft = g_slice_new(RFunctionType);
				new (ft) RFunctionType();
				ret = (RType *) ft;
				break;
			}
			default:
				ret = g_slice_new(RType);
				new (ret) RType();
		}
	}

	ret->proxy = proxy;
	ret->id = id;

	return ret;
}

RPrimitiveType *r_type_as_primitive( RType *type, GError **err ) {
	RPrimitiveType *bt;
	switch( type->id ) {
		case R_TYPE_CLASS_PRIMITIVE:
			r_type_ref(type);
			bt = (RPrimitiveType *) type;
			break;
		default: {
			if( type->proxy->lldbGetBasicType() == R_PRIMITIVE_TYPE_INVALID ) {
				g_set_error(
					err,
					RUMINATE_ERROR,
					RUMINATE_ERROR_NO_PRIMITIVE_TYPE,
					"No primitive type found for type \"%s\"",
					type->name
				);
				return NULL;
			}

			RType *_bt = r_type_new(type->proxy->getPrimitiveType(), err);
			if( _bt == NULL ) return NULL;

			bt = r_type_as_primitive(_bt, err);
			r_type_unref(_bt);
			if( _bt == NULL ) return NULL;
		}
	}
	return bt;
}

RStructType *r_type_as_struct( RType *type, GError **err ) {
	if( type->id != R_TYPE_CLASS_STRUCT ) {
		g_assert(false);
	}
	r_type_ref(type);
	return (RStructType *) type;
}

static void r_member_ref( RMember *m ) {
	g_atomic_int_inc(&m->refcnt);
}

void r_struct_member_ref( RStructMember *sm ) {
	r_member_ref((RMember *) sm);
}

void r_bit_field_member_ref( RBitFieldMember *bm ) {
	r_member_ref((RMember *) bm);
}

static bool r_member_unref( RMember *m ) {
	if( g_atomic_int_dec_and_test(&m->refcnt) ) {
		r_type_unref(m->type);
		g_free((char *) m->name);
		return true;
	}
	return false;
}

void r_struct_member_unref( RStructMember *sm ) {
	if( r_member_unref((RMember *) sm) ) {
		sm->~RStructMember();
		g_slice_free(RStructMember, sm);
	}
}

void r_bit_field_member_unref( RBitFieldMember *bm ) {
	if( r_member_unref((RMember *) bm) ) {
		bm->~RBitFieldMember();
		g_slice_free(RBitFieldMember, bm);
	}
}

static Ruminate::TypeListPrx container_type_memberlist( RContainerType *type ) {
	if( type->memberlist == 0 )
		type->memberlist = ((RType *) type)->proxy->getMembers();
	return type->memberlist;
}

RStructMember *r_struct_type_member_at_index( RStructType *type, size_t idx, GError **err ) {
	RStructMember *ret = g_slice_new(RStructMember);
	new (ret) RStructMember();
	RMember *mret = (RMember *) ret;
	((RMember *) ret)->proxy = container_type_memberlist((RContainerType *) type)->getTypeAtIndex(idx);

	return ret;
}

RBitFieldMember *r_bit_field_type_member_at_index( RBitFieldType *type, size_t idx, GError **err ) {
	RBitFieldMember *ret = g_slice_new(RBitFieldMember);
	new (ret) RBitFieldMember();
	((RMember *) ret)->proxy = container_type_memberlist((RContainerType *) type)->getTypeAtIndex(idx);

	return ret;
}

RType *r_type_pointee( RType *type, GError **err ) {
	if( type->id != R_TYPE_CLASS_POINTER ) {
		g_set_error_literal(
			err,
			RUMINATE_ERROR,
			RUMINATE_ERROR_INVALID_TYPE,
			"Supplied type is not a pointer"
		);
		return NULL;
	}

	return r_type_new(type->proxy->getPointeeType(), err);
}

RType *r_type_as_canonical( RType *type, GError **err ) {
	Ruminate::TypePrx proxy(type->proxy->getCanonicalType());
	if( proxy == 0 ) {
		r_type_ref(type);
		return type;
	}

	return r_type_new(proxy, err);
}

RFunctionType *r_type_as_function( RType *type, GError **err ) {
	if( type->id != R_TYPE_CLASS_FUNCTION ) {
		// TODO
		g_assert_cmpuint(type->id, ==, R_TYPE_CLASS_FUNCTION);
	}
	r_type_ref(type);
	return (RFunctionType *) type;
}

RBitFieldType *r_type_as_bit_field( RType *type, GError **err ) {
	if( !type->proxy->lldbIsBitfield() ) {
		// TODO
		g_assert(false);
	}
	r_type_ref(type);
	return (RBitFieldType *) type;
}

static Ruminate::TypeListPrx r_function_type_arglist( RFunctionType *type ) {
	if( type->arglist == 0 )
		type->arglist = ((RType *) type)->proxy->getFunctionArguments();
	return type->arglist;
}

RType *r_function_argument_type_at_index( RFunctionType *type, size_t idx, GError **err ) {
	;
	return r_type_new(r_function_type_arglist(type)->getTypeAtIndex(idx), err);
}

RType *r_function_return_type( RFunctionType *type, GError **err ) {
	return r_type_new(((RType *) type)->proxy->getFunctionReturnType(), err);
}

const char *r_type_name( RType *type, GError **err ) {
	if( type->name == NULL )
		type->name = g_strdup(type->proxy->getName().c_str());
	return type->name;
}

size_t r_type_size( RType *type, GError **err ) {
	uint64_t size = type->proxy->getSize();
	if( size > SIZE_MAX ) {
		g_set_error_literal(
			err,
			RUMINATE_ERROR,
			RUMINATE_ERROR_RANGE,
			"Type size too large to fit in size_t"
		);
		// TODO: Free previously allocated resources
		return 0;
	}
	return (size_t) size;
}

RTypeIdentifier r_type_id( RType *type, GError **err ) {
	return type->id;
}

size_t r_struct_type_nmembers( RStructType *type, GError **err ) {
#if 0
	if( !((RType *) type)->proxy->isComplete() ) {
		g_set_error_literal(
			err,
			RUMINATE_ERROR,
			RUMINATE_ERROR_INCOMPLETE_TYPE,
			"Incomplete type"
		);
		return 0;
	}
#endif
	RContainerType *ctype = (RContainerType *) type;
	if( ctype->memberlist == 0 )
		ctype->memberlist = ((RType *) type)->proxy->getMembers();
	return ctype->memberlist->getLength();
}

size_t r_bit_field_type_nmembers( RBitFieldType *type, GError **err ) {
	RContainerType *ctype = (RContainerType *) type;
	if( ctype->memberlist == 0 )
		ctype->memberlist = ((RType *) type)->proxy->getMembers();
	return ctype->memberlist->getLength();
}

RPrimitiveTypeIdentifier r_primitive_type_id( RPrimitiveType *type, GError **err ) {
	static_assert(sizeof(RPrimitiveTypeIdentifier) <= sizeof(uint32_t), "An RPrimitiveTypeIdentifier cannot fit within a uint32_t");
	return (RPrimitiveTypeIdentifier) ((RType *) type)->proxy->lldbGetBasicType();
}

size_t r_function_type_narguments( RFunctionType *type, GError **err ) {
	return r_function_type_arglist(type)->getLength();
}

static const char *r_member_name( RMember *ct, GError **err ) {
	if( ct->name == NULL )
		ct->name = g_strdup(ct->proxy->getName().c_str());
	return ct->name;
}

const char *r_struct_member_name( RStructMember *sm, GError **err ) {
	return r_member_name((RMember *) sm, err);
}

const char *r_bit_field_member_name( RBitFieldMember *bm, GError **err ) {
	return r_member_name((RMember *) bm, err);
}

off_t r_bit_field_member_offset_bits( RBitFieldMember *bm, GError **error ) {
	uint64_t offset = ((RMember *) bm)->proxy->lldbGetOffsetInBits();
	// TODO: OFF_MAX
	if( offset > SIZE_MAX ) {
		g_set_error_literal(
			error,
			RUMINATE_ERROR,
			RUMINATE_ERROR_RANGE,
			"Field offset too large to fit in size_t"
		);
		return 0;
	}
	return offset;
}

off_t r_struct_member_offset( RStructMember *sm, GError **err ) {
	uint64_t offset = ((RMember *) sm)->proxy->lldbGetOffsetInBytes();
	// TODO: OFF_MAX
	if( offset > SIZE_MAX ) {
		g_set_error_literal(
			err,
			RUMINATE_ERROR,
			RUMINATE_ERROR_RANGE,
			"Field offset too large to fit in size_t"
		);
		return 0;
	}
	return offset;
}

static RType *r_member_type( RMember *m, GError **error ) {
	if( m->type == NULL )
		m->type = r_type_new(m->proxy->lldbGetType(), error);
	return m->type;
}

RType *r_struct_member_type( RStructMember *sm, GError **err ) {
	return r_member_type((RMember *) sm, err);
}

RType *r_bit_field_member_type( RBitFieldMember *bm, GError **err ) {
	return r_member_type((RMember *) bm, err);
}

size_t r_bit_field_size_bits( RBitFieldType *bf, GError **error ) {
	uint64_t size = ((RType *) bf)->proxy->lldbGetBitfieldSizeInBits();
	if( size > SIZE_MAX ) {
		g_set_error_literal(
			error,
			RUMINATE_ERROR,
			RUMINATE_ERROR_RANGE,
			"Type size too large to fit in size_t"
		);
		// TODO: Free previously allocated resources
		return 0;
	}
	return (size_t) size;
}

RType *r_type_as_pointer( RType *type, GError **error ) {
	return r_type_new(type->proxy->getPointerType(), error);
}

G_END_DECLS
