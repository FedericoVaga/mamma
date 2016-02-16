#include <stdlib.h>
#include <mamma.h>

static void skel_set_up(struct m_suite *m_suite)
{
	/* Set up the environment */
}
static void skel_tear_down(struct m_suite *m_suite)
{
	/* Clean the environment */
}

static void skel_test1(struct m_test *m_test)
{
	/* First Test */
}

static void skel_test2(struct m_test *m_test)
{
	/* Second Test */
}


int main(int argc, char *argv[])
{
	struct m_test skel_tests[] = {
		m_test(NULL, skel_test1, NULL),
		m_test(NULL, skel_test2, NULL),
	};
	struct m_suite skel_suite = {
		.name = "skeleton test",
		.flags = 0,
		.tests = skel_tests,
		.test_count = ARRAY_SIZE(skel_tests),
		.set_up = skel_set_up,
		.tear_down = skel_tear_down,
	};

	m_suite_run(&skel_suite);

	return 0;
}
