#ifndef _RUMINATE_TYPE_H_
#define _RUMINATE_TYPE_H_

#include <glib.h>

struct Type;
typedef struct Type Type;

G_BEGIN_DECLS

void type_delete( Type **type );

const char *type_name( Type *type );

G_END_DECLS

#endif
