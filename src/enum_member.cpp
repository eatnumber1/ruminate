#include "config.h"

#include <exception>
#include <sstream>
#include <cstddef>
#include <new>

#include <stdint.h>

#include <Ice/Ice.h>
#include "ice/type.h"
#include "ice/type_member.h"

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/type.h"
#include "ruminate/builtin_type.h"
#include "ruminate/type_member.h"
#include "ruminate/aggregate_member.h"
#include "ruminate/enum_member.h"

#include "private/common.h"
#include "private/string.h"
#include "private/type.h"
#include "private/type_member.h"
#include "private/aggregate_member.h"
#include "private/enum_member.h"

static bool ensure_value_init( REnumMember *em, GError **error ) RUMINATE_NOEXCEPT {
	RType *rt = NULL;
	RBuiltinType *rbt = NULL;
	RTypeMember *rm = NULL;
	bool signd = false;
	intmax_t val = 0;
	GError *err = NULL;

	if( em->value.initialized ) return true;

	rm = (RTypeMember *) em;

	rt = r_type_member_type(rm, &err);
	if( rt == NULL ) goto error_r_type_member_type;
	g_assert(r_type_id(rt, NULL) == R_TYPE_BUILTIN);
	rbt = (RBuiltinType *) rt;

	signd = r_builtin_type_is_signed(rbt, &err);
	if( err != NULL ) goto error_r_builtin_type_is_signed;

	if( !gxx_call(val = rm->member->getValueSigned(), &err) )
		goto error_get_value_signed;

	if( signd ) {
		em->value.type = ENUM_MEMBER_VALUE_TYPE_SIGNED;
		em->value.signd = val;
	} else {
		em->value.type = ENUM_MEMBER_VALUE_TYPE_UNSIGNED;
		em->value.unsignd = (uintmax_t) val;
	}

	em->value.initialized = true;
	return true;

error_get_value_signed:
error_r_builtin_type_is_signed:
	r_type_unref(rt);
error_r_type_member_type:
	g_propagate_error(error, err);
	return false;
}

bool r_enum_member_init( REnumMember *em, RType *, GError ** ) RUMINATE_NOEXCEPT {
	em->value.initialized = false;

	return true;
}

void r_enum_member_destroy( REnumMember * ) RUMINATE_NOEXCEPT {}

REnumMember *r_enum_member_alloc( RType *, GError ** ) RUMINATE_NOEXCEPT {
	REnumMember *ret = g_slice_new(REnumMember);
	new (ret) REnumMember();
	return ret;
}

void r_enum_member_free( REnumMember *em ) RUMINATE_NOEXCEPT {
	em->~REnumMember();
	g_slice_free(REnumMember, em);
}

G_BEGIN_DECLS

uintmax_t r_enum_member_value_signed( REnumMember *em, GError **error ) RUMINATE_NOEXCEPT {
	if( !ensure_value_init(em, error) ) return 0;
	if( em->value.type != ENUM_MEMBER_VALUE_TYPE_SIGNED ) {
		// TODO: Error here.
		g_assert(false);
	}
	return em->value.signd;
}

intmax_t r_enum_member_value_unsigned( REnumMember *em, GError **error ) RUMINATE_NOEXCEPT {
	if( !ensure_value_init(em, error) ) return 0;
	if( em->value.type != ENUM_MEMBER_VALUE_TYPE_UNSIGNED )  {
		// TODO: Error here.
		g_assert(false);
	}
	return em->value.unsignd;
}

G_END_DECLS
