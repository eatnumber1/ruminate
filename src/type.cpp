#include "private/type.h"

#include "ruminate/errors.h"

#include <Ice/Ice.h>
#include "ice/type.h"

#include <glib.h>

#include <exception>

struct Type {
	Ruminate::TypePrx proxy;
};

Type *type_new( Ruminate::TypePrx proxy, GError **err ) {
	Type *ret = g_slice_new(Type);
	new (ret) Type();

	ret->proxy = proxy;

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
	return type->proxy->getName().c_str();
}
