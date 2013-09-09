#ifndef _RUMINATE_H_
#define _RUMINATE_H_

#include <glib.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

G_BEGIN_DECLS

#include <ruminate/common.h>
#include <ruminate/string.h>
#include <ruminate/errors.h>
#include <ruminate/refptr.h>

#include <ruminate/type.h>
#include <ruminate/type_member.h>
#include <ruminate/record_member.h>
#include <ruminate/builtin_type.h>
#include <ruminate/array_type.h>
#include <ruminate/typedef_type.h>
#include <ruminate/pointer_type.h>
#include <ruminate/tag_type.h>
#include <ruminate/record_type.h>
#include <ruminate/function_type.h>

#include <ruminate/frame.h>
#include <ruminate/rumination.h>

G_END_DECLS

#endif
