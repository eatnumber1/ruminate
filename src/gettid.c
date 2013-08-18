#define _GNU_SOURCE

#include <unistd.h>

#ifdef __linux__
#include <sys/syscall.h>
#else
//#error Don't know how to get tid on this platform
#endif

#include "private/glib.h"

#include "private/gettid.h"

pid_t gettid() {
#ifdef __linux__
	return syscall(SYS_gettid);
#else
#warning REMOVEME
	return (pid_t) 0;
#endif
}
