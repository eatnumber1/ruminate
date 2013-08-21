#define _GNU_SOURCE

#include <unistd.h>
#include <stdint.h>

#if defined(__linux__) || ( defined(__APPLE__) && defined(__MACH__) )
#include <sys/syscall.h>
#else
#error Don't know how to get tid on this platform
#endif

#include "private/glib.h"

#include "private/gettid.h"

uint64_t gettid() {
#if defined(__linux__)
	return syscall(SYS_gettid);
#elif defined(__APPLE__) && defined(__MACH__)
	return syscall(SYS_thread_selfid);
#else
#error Don't know how to get tid on this platform
#endif
}
