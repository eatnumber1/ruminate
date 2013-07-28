#define _GNU_SOURCE

#include <unistd.h>

#ifdef __linux__
#include <sys/syscall.h>
#else
#error Don't know how to get tid on this platform
#endif

#include <glib.h>

#include "private/gettid.h"

pid_t gettid() {
	return syscall(SYS_gettid);
}
