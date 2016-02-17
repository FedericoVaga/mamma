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


static void skel_test_setup(struct m_test *m_test)
{
	/* Test set up */
}

static void skel_test_tear_down(struct m_test *m_test)
{
	/* Test tear down */
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
		m_test(skel_test_setup, skel_test1, skel_test_tear_down),
		m_test(NULL, skel_test2, NULL),
	};
	struct m_suite skel_suite = m_suite("skeleton test", 0, skel_tests,
					    skel_set_up, skel_tear_down);
	m_suite_run(&skel_suite);

	return 0;
}
