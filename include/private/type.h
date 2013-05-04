#include <lldb/API/LLDB.h>
#include <glib.h>

G_BEGIN_DECLS

Type *type_new( lldb::SBType &type, GError **err );

G_END_DECLS
