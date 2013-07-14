#define _RUMINATE_TYPE_CPP_

typedef struct RTypePrivate RTypePrivate;
typedef struct RStructMemberPrivate RStructMemberPrivate;
typedef struct RStructTypePrivate RStructTypePrivate;
typedef struct RFunctionTypePrivate RFunctionTypePrivate;

#include "private/type.h"

#include "ruminate/errors.h"

#include <Ice/Ice.h>
#include "ice/type.h"
#include "ice/type_list.h"

#include <glib.h>

#include <exception>

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

typedef struct RTypePrivate {
	gint refcnt;
	Ruminate::TypePrx proxy;
} TypePrivate;

typedef struct RStructMemberPrivate {
	gint refcnt;
	Ruminate::TypePrx proxy;
} StructMemberPrivate;

typedef struct RStructTypePrivate {
	Ruminate::TypeListPrx memberlist;
} StructTypePrivate;

typedef struct RFunctionTypePrivate {
	Ruminate::TypeListPrx arglist;
} FunctionTypePrivate;

G_BEGIN_DECLS

void r_type_unref( RType *type ) {
	RTypePrivate *tp = type->priv;
	if( g_atomic_int_dec_and_test(&tp->refcnt) ) {
		tp->~RTypePrivate();
		g_slice_free(RTypePrivate, tp);

		g_free((char *) type->name);

		switch( type->id ) {
			case R_TYPE_CLASS_STRUCT: {
				RStructType *st = (RStructType *) type;
				RStructTypePrivate *priv = st->priv;

				priv->~RStructTypePrivate();
				st->~RStructType();

				g_slice_free(RStructTypePrivate, priv);
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
				RFunctionTypePrivate *priv = ft->priv;

				priv->~RFunctionTypePrivate();
				ft->~RFunctionType();

				g_slice_free(RFunctionTypePrivate, priv);
				g_slice_free(RFunctionType, ft);
			}
			default:
				type->~RType();
				g_slice_free(RType, type);
		}
	}
}

void r_type_ref( RType *type ) {
	g_atomic_int_inc(&type->priv->refcnt);
}

RType *r_type_new( Ruminate::TypePrx proxy, GError **err ) {
	static_assert(sizeof(RTypeIdentifier) <= sizeof(uint32_t), "An RTypeIdentifier can't fit in a uint32_t!");
	RTypeIdentifier id = (RTypeIdentifier) proxy->lldbGetTypeClass();

	RType *ret = NULL;
	switch( id ) {
		case R_TYPE_CLASS_STRUCT: {
			RStructType *st = g_slice_new(RStructType);
			new (st) RStructType();
			st->priv = g_slice_new(RStructTypePrivate);
			new (st->priv) RStructTypePrivate();
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
			ft->priv = g_slice_new(RFunctionTypePrivate);
			new (ft->priv) RFunctionTypePrivate();
			ret = (RType *) ft;
			break;
		}
		default:
			ret = g_slice_new(RType);
			new (ret) RType();
	}

	ret->priv = g_slice_new(RTypePrivate);
	new (ret->priv) RTypePrivate();

	RTypePrivate *tp = ret->priv;
	tp->proxy = proxy;
	tp->refcnt = 1;

	uint64_t size = proxy->getSize();
	if( size > SIZE_MAX ) {
		g_set_error_literal(
			err,
			RUMINATE_ERROR,
			RUMINATE_ERROR_RANGE,
			"Type size too large to fit in size_t"
		);
		// TODO: Free previously allocated resources
		return NULL;
	}
	ret->size = size;

	ret->id = id;
	ret->name = g_strdup(proxy->getName().c_str());

	switch( ret->id ) {
		case R_TYPE_CLASS_STRUCT: {
			RStructType *st = (RStructType *) ret;
			RStructTypePrivate *priv = st->priv;

			priv->memberlist = proxy->getStructFields();
			st->nfields = priv->memberlist->getLength();
			break;
		}
		case R_TYPE_CLASS_PRIMITIVE: {
			RPrimitiveType *bt = (RPrimitiveType *) ret;
			static_assert(sizeof(RPrimitiveTypeIdentifier) <= sizeof(uint32_t), "An RPrimitiveTypeIdentifier cannot fit within a uint32_t");
			bt->id = (RPrimitiveTypeIdentifier) proxy->lldbGetBasicType();
			break;
		}
		case R_TYPE_CLASS_FUNCTION: {
			RFunctionType *ft = (RFunctionType *) ret;
			RFunctionTypePrivate *priv = ft->priv;

			priv->arglist = proxy->getFunctionArguments();
			ft->narguments = priv->arglist->getLength();
			break;
		}
		default:
			break;
	}

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
			RTypePrivate *priv = type->priv;
			if( priv->proxy->lldbGetBasicType() == R_PRIMITIVE_TYPE_INVALID ) {
				g_set_error(
					err,
					RUMINATE_ERROR,
					RUMINATE_ERROR_NO_PRIMITIVE_TYPE,
					"No primitive type found for type \"%s\"",
					type->name
				);
				return NULL;
			}

			RType *_bt = r_type_new(priv->proxy->getPrimitiveType(), err);
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

void r_struct_member_ref( RStructMember *sm ) {
	g_atomic_int_inc(&((RType *)sm)->priv->refcnt);
}

void r_struct_member_unref( RStructMember *sm ) {
	RStructMemberPrivate *smp = sm->priv;
	if( g_atomic_int_dec_and_test(&smp->refcnt) ) {
		r_type_unref(sm->type);
		smp->~RStructMemberPrivate();
		g_slice_free(RStructMemberPrivate, smp);
		g_free((char *) sm->name);
		sm->~RStructMember();
		g_slice_free(RStructMember, sm);
	}
}

RStructMember *r_struct_type_field_at_index( RStructType *type, size_t idx, GError **err ) {
	RStructMember *ret = g_slice_new(RStructMember);
	new (ret) RStructMember();

	RStructMemberPrivate *priv = g_slice_new(RStructMemberPrivate);
	new (priv) RStructMemberPrivate();

	priv->proxy = type->priv->memberlist->getTypeAtIndex(idx);
	priv->refcnt = 1;

	uint64_t offset = priv->proxy->lldbGetOffsetInBytes();
	if( offset > SIZE_MAX ) {
		g_set_error_literal(
			err,
			RUMINATE_ERROR,
			RUMINATE_ERROR_RANGE,
			"Field offset too large to fit in size_t"
		);
		goto out_lldb_get_offset_in_bytes;
	}
	ret->offset = offset;

	ret->type = r_type_new(priv->proxy->lldbGetType(), err);
	if( ret->type == NULL ) goto out_type_new;

	ret->name = g_strdup(priv->proxy->getName().c_str());

	ret->priv = priv;

	return ret;

out_type_new:
out_lldb_get_offset_in_bytes:
	priv->~RStructMemberPrivate();
	g_slice_free(RStructMemberPrivate, priv);
	ret->~RStructMember();
	g_slice_free(RStructMember, ret);
	return NULL;
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

	return r_type_new(type->priv->proxy->getPointeeType(), err);
}

RType *r_type_as_canonical( RType *type, GError **err ) {
	Ruminate::TypePrx proxy(type->priv->proxy->getCanonicalType());
	if( proxy == 0 ) {
		r_type_ref(type);
		return type;
	}

	return r_type_new(proxy, err);
}

RFunctionType *r_type_as_function( RType *type, GError **err ) {
	if( type->id != R_TYPE_CLASS_FUNCTION ) {
		g_assert_cmpuint(type->id, ==, R_TYPE_CLASS_FUNCTION);
	}
	r_type_ref(type);
	return (RFunctionType *) type;
}

RType *r_function_argument_type_at_index( RFunctionType *type, size_t idx, GError **err ) {
	return r_type_new(type->priv->arglist->getTypeAtIndex(idx), err);
}

RType *r_function_return_type( RFunctionType *type, GError **err ) {
	return r_type_new(((RType *) type)->priv->proxy->getFunctionReturnType(), err);
}

G_END_DECLS
