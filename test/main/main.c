#include <ruminate.h>

#include <glib.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#if 0
typedef struct SubprocessTestFunction {
	void (*parent)( int );
	GTestFunc child;
} SubprocessTestFunction;

static void assert_child_success( int stat ) {
	g_assert(WIFEXITED(stat));
	g_assert(WEXITSTATUS(stat) == 0);
}

static void assert_child_fail_signal( int stat, int sig ) {
	g_assert(WIFSIGNALED(stat));
	g_assert(WTERMSIG(stat) == sig);
}

static void test_with_subprocess( SubprocessTestFunction *sptf ) {
	pid_t pid = fork();
	g_assert_cmpint(pid, !=, -1);
	if( pid == 0 ) {
		sptf->child();
	} else {
		int stat;
		pid_t ret = waitpid(pid, &stat, 0);
		g_assert_cmpint(ret, !=, -1);
		sptf->parent(stat);
	}
}

static void do_ruminate_init() {
	GError *err = NULL;
	int argc = 1;
	char *argv[] = { g_get_prgname() };

	ruminate_init(&argc, argv, &err);
	g_assert_no_error(err);
}

static void do_ruminate_destroy() {
	GError *err = NULL;

	ruminate_destroy(&err);
	g_assert_no_error(err);
}

static void test_ruminate_init_subprocess() {
	do_ruminate_init();
	do_ruminate_destroy();
}

static void test_ruminate_init( int stat ) {
	assert_child_success(stat);
}

static void test_signal_delivery_subprocess() {
	do_ruminate_init();
	kill(getpid(), SIGINT);
	g_assert(false);
}

static void test_signal_delivery( int stat ) {
	assert_child_fail_signal(stat, SIGINT);
}

int main( int argc, char **argv ) {
	g_test_init(&argc, &argv, NULL);

	SubprocessTestFunction test_ruminate_init_data = {
		.parent = test_ruminate_init,
		.child = test_ruminate_init_subprocess
	};
	g_test_add_data_func("/start", &test_ruminate_init_data, (GTestDataFunc) test_with_subprocess);


	SubprocessTestFunction test_signal_delivery_data = {
		.parent = test_signal_delivery,
		.child = test_signal_delivery_subprocess
	};
	g_test_add_data_func("/signal", &test_signal_delivery_data, (GTestDataFunc) test_with_subprocess);

	return g_test_run();
}
#endif

static void do_ruminate_init( char **prgname ) {
	GError *err = NULL;
	*prgname = g_strdup(g_get_prgname());

	ruminate_init(*prgname, &err);
	g_assert_no_error(err);
}

static void do_ruminate_destroy( char **prgname ) {
	GError *err = NULL;

	ruminate_destroy(&err);
	g_assert_no_error(err);
	g_free(*prgname);
}

#if 0
static void test_ruminate_init_and_destroy_subprocess() {
	char *prgname;
	do_ruminate_init(&prgname);
	do_ruminate_destroy(&prgname);
}
#endif

static void test_ruminate_init_and_destroy() {
	//g_test_trap_subprocess("/init_and_destroy/subprocess", 5 * G_USEC_PER_SEC, G_TEST_SUBPROCESS_INHERIT_STDERR | G_TEST_SUBPROCESS_INHERIT_STDOUT);
	//g_test_trap_assert_passed();

	char *prgname = NULL;
	do_ruminate_init(&prgname);
	do_ruminate_destroy(&prgname);
}

int main( int argc, char **argv ) {
	g_test_init(&argc, &argv, NULL);
	test_ruminate_init_and_destroy();
	exit(0);
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/init_and_destroy", test_ruminate_init_and_destroy);
	//g_test_add_func("/init_and_destroy/subprocess", test_ruminate_init_and_destroy_subprocess);

	return g_test_run();
}
