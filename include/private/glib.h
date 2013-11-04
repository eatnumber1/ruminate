#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wdeprecated-register"

#include <glib.h>

#pragma clang diagnostic pop

#define R_STATIC_ASSERT(cond) \
	_Pragma("clang diagnostic push") \
	_Pragma("clang diagnostic ignored \"-Wvla-extension\"") \
	_Pragma("clang diagnostic ignored \"-Wpedantic\"") \
	G_STATIC_ASSERT(cond) \
	_Pragma("clang diagnostic pop")
