#define _GNU_SOURCE

#include "private/gettid.h"

#include <unistd.h>
#include <sys/syscall.h>

pid_t gettid() {
	return syscall(SYS_gettid);
}
