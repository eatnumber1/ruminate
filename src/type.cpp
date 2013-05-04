#include "ruminate/type.h"
#include "private/type.h"

#include "ruminate/errors.h"

#include <lldb/API/LLDB.h>
#include <glib.h>

#include <exception>

using namespace lldb;

struct Type {
	SBType sb;
};

Type *type_new( SBType &type, GError **err ) {
	Type *ret = g_slice_new(Type);
	new (ret) Type();

	ret->sb = type;

	if( !ret->sb.IsValid() ) {
		g_set_error(
			err,
			RUMINATE_ERROR,
			RUMINATE_ERROR_SB_INVALID,
			"SBType instance invalid"
		);
		ret->~Type();
		g_slice_free(Type, ret);
		return NULL;
	}

	return ret;
}

void type_delete( Type **type ) {
	g_return_if_fail(type != NULL && *type != NULL);

	Type *t = *type;
	t->~Type();
	g_slice_free(Type, t);
	*type = NULL;
}

const char *type_name( Type *type ) {
	return type->sb.GetName();
}
