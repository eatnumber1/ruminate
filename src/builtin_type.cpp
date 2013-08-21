#include <exception>
#include <sstream>
#include <cstddef>

#include <Ice/Ice.h>
#include "ice/type.h"

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/type.h"
#include "ruminate/builtin_type.h"

#define _BUILTIN_TYPE_CPP_

#include "private/common.h"
#include "private/type.h"
#include "private/builtin_type.h"

bool r_builtin_type_init( RBuiltinType *rbt, GError ** ) RUMINATE_NOEXCEPT {
	switch( ((RType *) rbt)->type_id ) {
		case Ruminate::TypeIdInt:
			rbt->id = R_BUILTIN_TYPE_INT;
			break;
		case Ruminate::TypeIdLong:
			rbt->id = R_BUILTIN_TYPE_LONG;
			break;
		case Ruminate::TypeIdDouble:
			rbt->id = R_BUILTIN_TYPE_DOUBLE;
			break;
		case Ruminate::TypeIdVoid:
			rbt->id = R_BUILTIN_TYPE_VOID;
			break;
		case Ruminate::TypeIdShort:
			rbt->id = R_BUILTIN_TYPE_SHORT;
			break;
		case Ruminate::TypeIdChar:
			rbt->id = R_BUILTIN_TYPE_CHAR;
			break;
		default:
			g_assert_not_reached();
	}

	return true;
}

void r_builtin_type_destroy( RBuiltinType * ) RUMINATE_NOEXCEPT {}

RBuiltinType *r_builtin_type_alloc( Ruminate::TypeId, GError ** ) RUMINATE_NOEXCEPT {
	RBuiltinType *ret = g_slice_new(RBuiltinType);
	new (ret) RBuiltinType();
	return ret;
}

void r_builtin_type_free( RBuiltinType *rbt ) RUMINATE_NOEXCEPT {
	rbt->~RBuiltinType();
	g_slice_free(RBuiltinType, rbt);
}

G_BEGIN_DECLS

RBuiltinTypeId r_builtin_type_id( RBuiltinType *rbt, GError ** ) RUMINATE_NOEXCEPT {
	return rbt->id;
}

bool r_builtin_type_is_signed( RBuiltinType *rbt, GError **error ) RUMINATE_NOEXCEPT {
	bool ret = false;
	(void) gxx_call(ret = ((RType *) rbt)->type->isSigned(), error);
	return ret;
}

bool r_builtin_type_is_unsigned( RBuiltinType *rbt, GError **error ) RUMINATE_NOEXCEPT {
	bool ret = false;
	(void) gxx_call(ret = ((RType *) rbt)->type->isUnsigned(), error);
	return ret;
}

G_END_DECLS
