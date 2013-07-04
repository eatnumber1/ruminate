#define _GNU_SOURCE

#include "private/gettid.h"

#include <unistd.h>

#ifdef __linux__
#include <sys/syscall.h>
#else
//#error Don't know how to get tid on this platform
#endif

pid_t gettid() {
//	return syscall(SYS_gettid);
	return 0;
}
