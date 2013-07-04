#include "ruminate/type.h"

#include <Ice/Ice.h>
#include "ice/type.h"

#include <glib.h>

G_BEGIN_DECLS

Type *type_new( Ruminate::TypePrx &, GError **err );

G_END_DECLS
