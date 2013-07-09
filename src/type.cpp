#include "private/type.h"

#include "ruminate/errors.h"

#include <Ice/Ice.h>
#include "ice/type.h"

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

static TypePrivate &priv( Type &type ) {
	return *((TypePrivate *) type.priv);
}

static TypePrivate &priv( Type *type ) {
	return priv(*type);
}

static StructMemberPrivate &priv( StructMember &sm ) {
	return *((StructMemberPrivate *) sm.priv);
}

static StructMemberPrivate &priv( StructMember *sm ) {
	return priv(*sm);
}

#define TYPE_PTR_PRIV(p) (::priv((Type *) p))
#define TYPE_PRIV(p) (::priv((Type) p))
#define STRUCT_MEMBER_PTR_PRIV(p) (::priv((StructMember *) p))
#define STRUCT_MEMBER_PRIV(p) (::priv((StructMember) p))

G_BEGIN_DECLS

void type_unref( Type *type ) {
	TypePrivate &tp = TYPE_PTR_PRIV(type);
	if( g_atomic_int_dec_and_test(&tp.refcnt) ) {
		tp.~TypePrivate();
		g_slice_free(TypePrivate, &tp);

		switch( type->id ) {
			case TYPE_CLASS_STRUCT: {
				StructType *st = (StructType *) type;
				st->~StructType();
				g_slice_free(StructType, st);
				break;
			}
			default:
				type->~Type();
				g_slice_free(Type, type);
		}
	}
}

void type_ref( Type *type ) {
	g_atomic_int_inc(&TYPE_PTR_PRIV(type).refcnt);
}

Type *type_new( Ruminate::TypePrx proxy, GError **err ) {
	static_assert(sizeof(TypeIdentifier) <= sizeof(uint32_t), "A TypeIdentifier can't fit in a uint32_t!");
	TypeIdentifier id = (TypeIdentifier) proxy->lldbGetTypeClass();

	Type *ret = NULL;
	switch( id ) {
		case TYPE_CLASS_STRUCT:
			ret = (Type *) g_slice_new(StructType);
			new (ret) StructType();
			break;
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
	ret->name = proxy->getName().c_str();

	switch( ret->id ) {
		case TYPE_CLASS_STRUCT: {
			StructType *st = (StructType *) ret;
			st->nfields = proxy->lldbGetNumberOfFields();
			break;
		}
		default:
			break;
	}

	return ret;
}

Type *type_basic( Type *type, GError **err ) {
	return type_new(TYPE_PTR_PRIV(type).proxy->getBasicType(), err);
}

StructType *type_as_struct( Type *type, GError **err ) {
	if( type->id != TYPE_CLASS_STRUCT ) {
		g_assert(false);
	}
	type_ref(type);
	return (StructType *) type;
}

void struct_member_ref( StructMember *sm ) {
	g_atomic_int_inc(&TYPE_PTR_PRIV(sm).refcnt);
}

void struct_member_unref( StructMember *sm ) {
	StructMemberPrivate &smp = STRUCT_MEMBER_PTR_PRIV(sm);
	if( g_atomic_int_dec_and_test(&smp.refcnt) ) {
		type_unref(sm->type);
		smp.~StructMemberPrivate();
		g_slice_free(StructMemberPrivate, sm->priv);
		sm->~StructMember();
		g_slice_free(StructMember, sm);
	}
}

StructMember *struct_type_field_at_index( StructType *type, size_t idx, GError **err ) {
	StructMember *ret = g_slice_new(StructMember);
	new (ret) StructMember();

	StructMemberPrivate *priv = g_slice_new(StructMemberPrivate);
	new (priv) StructMemberPrivate();

	priv->proxy = TYPE_PTR_PRIV(type).proxy->lldbGetFieldAtIndex(idx);
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

	ret->name = priv->proxy->getName().c_str();

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

G_END_DECLS
