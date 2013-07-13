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

typedef struct TypePrivate {
	gint refcnt;
	Ruminate::TypePrx proxy;
} TypePrivate;

typedef struct StructMemberPrivate {
	gint refcnt;
	Ruminate::TypePrx proxy;
} StructMemberPrivate;

typedef struct StructTypePrivate {
	Ruminate::TypeListPrx memberlist;
} StructTypePrivate;

typedef struct FunctionTypePrivate {
	Ruminate::TypeListPrx arglist;
} FunctionTypePrivate;

static TypePrivate *priv( Type *type ) {
	return (TypePrivate *) type->priv;
}

static StructTypePrivate *priv( StructType *type ) {
	return (StructTypePrivate *) type->priv;
}

static FunctionTypePrivate *priv( FunctionType *type ) {
	return (FunctionTypePrivate *) type->priv;
}

static StructMemberPrivate *priv( StructMember *sm ) {
	return (StructMemberPrivate *) sm->priv;
}

#define TYPE_PRIV(p) (::priv((Type *) p))
#define STRUCT_TYPE_PRIV(p) (::priv((StructType *) p))
#define FUNCTION_TYPE_PRIV(p) (::priv((FunctionType *) p))

#define STRUCT_MEMBER_PRIV(p) (::priv((StructMember *) p))

G_BEGIN_DECLS

void type_unref( Type *type ) {
	TypePrivate *tp = TYPE_PRIV(type);
	if( g_atomic_int_dec_and_test(&tp->refcnt) ) {
		tp->~TypePrivate();
		g_slice_free(TypePrivate, tp);

		g_free((char *) type->name);

		switch( type->id ) {
			case TYPE_CLASS_STRUCT: {
				StructType *st = (StructType *) type;
				StructTypePrivate *priv = (StructTypePrivate *) st->priv;

				priv->~StructTypePrivate();
				st->~StructType();

				g_slice_free(StructTypePrivate, priv);
				g_slice_free(StructType, st);
				break;
			}
			case TYPE_CLASS_PRIMITIVE: {
				PrimitiveType *pt = (PrimitiveType *) type;
				pt->~PrimitiveType();
				g_slice_free(PrimitiveType, pt);
				break;
			}
			case TYPE_CLASS_FUNCTION: {
				FunctionType *ft = (FunctionType *) type;
				FunctionTypePrivate *priv = (FunctionTypePrivate *) ft->priv;

				priv->~FunctionTypePrivate();
				ft->~FunctionType();

				g_slice_free(FunctionTypePrivate, priv);
				g_slice_free(FunctionType, ft);
			}
			default:
				type->~Type();
				g_slice_free(Type, type);
		}
	}
}

void type_ref( Type *type ) {
	g_atomic_int_inc(&TYPE_PRIV(type)->refcnt);
}

Type *type_new( Ruminate::TypePrx proxy, GError **err ) {
	static_assert(sizeof(TypeIdentifier) <= sizeof(uint32_t), "A TypeIdentifier can't fit in a uint32_t!");
	TypeIdentifier id = (TypeIdentifier) proxy->lldbGetTypeClass();

	Type *ret = NULL;
	switch( id ) {
		case TYPE_CLASS_STRUCT: {
			StructType *st = g_slice_new(StructType);
			new (st) StructType();
			st->priv = g_slice_new(StructTypePrivate);
			new (st->priv) StructTypePrivate();
			ret = (Type *) st;
			break;
		}
		case TYPE_CLASS_PRIMITIVE:
			ret = (Type *) g_slice_new(PrimitiveType);
			new (ret) PrimitiveType();
			break;
		case TYPE_CLASS_FUNCTION: {
			FunctionType *ft = g_slice_new(FunctionType);
			new (ft) FunctionType();
			ft->priv = g_slice_new(FunctionTypePrivate);
			new (ft->priv) FunctionTypePrivate();
			ret = (Type *) ft;
			break;
		}
		default:
			ret = g_slice_new(Type);
			new (ret) Type();
	}

	ret->priv = g_slice_new(TypePrivate);
	new (ret->priv) TypePrivate();

	TypePrivate *tp = (TypePrivate *) ret->priv;
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
		case TYPE_CLASS_STRUCT: {
			StructType *st = (StructType *) ret;
			StructTypePrivate *priv = (StructTypePrivate *) st->priv;

			priv->memberlist = proxy->getStructFields();
			st->nfields = priv->memberlist->getLength();
			break;
		}
		case TYPE_CLASS_PRIMITIVE: {
			PrimitiveType *bt = (PrimitiveType *) ret;
			static_assert(sizeof(PrimitiveTypeIdentifier) <= sizeof(uint32_t), "A PrimitiveTypeIdentifier cannot fit within a uint32_t");
			bt->id = (PrimitiveTypeIdentifier) proxy->lldbGetBasicType();
			break;
		}
		case TYPE_CLASS_FUNCTION: {
			FunctionType *ft = (FunctionType *) ret;
			FunctionTypePrivate *priv = (FunctionTypePrivate *) ft->priv;

			priv->arglist = proxy->getFunctionArguments();
			ft->narguments = priv->arglist->getLength();
			break;
		}
		default:
			break;
	}

	return ret;
}

PrimitiveType *type_as_primitive( Type *type, GError **err ) {
	PrimitiveType *bt;
	switch( type->id ) {
		case TYPE_CLASS_PRIMITIVE:
			type_ref(type);
			bt = (PrimitiveType *) type;
			break;
		default: {
			TypePrivate *priv = TYPE_PRIV(type);
			if( priv->proxy->lldbGetBasicType() == PRIMITIVE_TYPE_INVALID ) {
				g_set_error(
					err,
					RUMINATE_ERROR,
					RUMINATE_ERROR_NO_PRIMITIVE_TYPE,
					"No primitive type found for type \"%s\"",
					type->name
				);
				return NULL;
			}

			Type *_bt = type_new(priv->proxy->getPrimitiveType(), err);
			if( _bt == NULL ) return NULL;

			bt = type_as_primitive(_bt, err);
			type_unref(_bt);
			if( _bt == NULL ) return NULL;
		}
	}
	return bt;
}

StructType *type_as_struct( Type *type, GError **err ) {
	if( type->id != TYPE_CLASS_STRUCT ) {
		g_assert(false);
	}
	type_ref(type);
	return (StructType *) type;
}

void struct_member_ref( StructMember *sm ) {
	g_atomic_int_inc(&TYPE_PRIV(sm)->refcnt);
}

void struct_member_unref( StructMember *sm ) {
	StructMemberPrivate *smp = STRUCT_MEMBER_PRIV(sm);
	if( g_atomic_int_dec_and_test(&smp->refcnt) ) {
		type_unref(sm->type);
		smp->~StructMemberPrivate();
		g_slice_free(StructMemberPrivate, smp);
		g_free((char *) sm->name);
		sm->~StructMember();
		g_slice_free(StructMember, sm);
	}
}

StructMember *struct_type_field_at_index( StructType *type, size_t idx, GError **err ) {
	StructMember *ret = g_slice_new(StructMember);
	new (ret) StructMember();

	StructMemberPrivate *priv = g_slice_new(StructMemberPrivate);
	new (priv) StructMemberPrivate();

	priv->proxy = STRUCT_TYPE_PRIV(type)->memberlist->getTypeAtIndex(idx);
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

	ret->type = type_new(priv->proxy->lldbGetType(), err);
	if( ret->type == NULL ) goto out_type_new;

	ret->name = g_strdup(priv->proxy->getName().c_str());

	ret->priv = priv;

	return ret;

out_type_new:
out_lldb_get_offset_in_bytes:
	priv->~StructMemberPrivate();
	g_slice_free(StructMemberPrivate, priv);
	ret->~StructMember();
	g_slice_free(StructMember, ret);
	return NULL;
}

Type *type_pointee( Type *type, GError **err ) {
	if( type->id != TYPE_CLASS_POINTER ) {
		g_set_error_literal(
			err,
			RUMINATE_ERROR,
			RUMINATE_ERROR_INVALID_TYPE,
			"Supplied type is not a pointer"
		);
		return NULL;
	}

	return type_new(TYPE_PRIV(type)->proxy->getPointeeType(), err);
}

Type *type_as_canonical( Type *type, GError **err ) {
	Ruminate::TypePrx proxy(TYPE_PRIV(type)->proxy->getCanonicalType());
	if( proxy == 0 ) {
		type_ref(type);
		return type;
	}

	return type_new(proxy, err);
}

FunctionType *type_as_function( Type *type, GError **err ) {
	if( type->id != TYPE_CLASS_FUNCTION ) {
		g_assert_cmpuint(type->id, ==, TYPE_CLASS_FUNCTION);
	}
	type_ref(type);
	return (FunctionType *) type;
}

Type *function_argument_type_at_index( FunctionType *type, size_t idx, GError **err ) {
	return type_new(FUNCTION_TYPE_PRIV(type)->arglist->getTypeAtIndex(idx), err);
}

Type *function_return_type( FunctionType *type, GError **err ) {
	return type_new(TYPE_PRIV(type)->proxy->getFunctionReturnType(), err);
}

G_END_DECLS
