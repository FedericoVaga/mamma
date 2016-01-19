/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */
#include <stdio.h>
#include <string.h>
#include "cut.h"


static void cut_test_run(struct cut_test *cut_test, unsigned long flags)
{
	if (cut_test->set_up)
		cut_test->set_up(cut_test);
	if (cut_test->test)
		cut_test->test(cut_test);
	if (cut_test->tear_down)
		cut_test->tear_down(cut_test);

	if (flags & CUT_VERBOSE)
		fprintf(stdout, "Test \"%s\"\n", cut_test->name);
}


/**
 * It creates a test suite, in other words a collection of tests.
 *
 * @param[in] name suite name
 */
void cut_suite_init(struct cut_suite *suite)
{
	int i;

	for (i = 0; i < suite->test_count; i++)
		suite->tests[i].suite = suite;
}


/**
 * It runs all the tests within the given suite
 * @param[in] cut_suite the suite to run
 */
void cut_suite_run(struct cut_suite *cut_suite, unsigned long flags)
{
	int i;

	if (flags & CUT_VERBOSE) {
		fprintf(stdout, "Running suite \"%s\"\n", cut_suite->name);
		fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	}
	if (cut_suite->set_up)
		cut_suite->set_up(cut_suite);
	for (i = 0; i < cut_suite->test_count; i++) {
		cut_test_run(&cut_suite->tests[i], flags);
	}
	if (cut_suite->tear_down)
		cut_suite->tear_down(cut_suite);

	if (flags & CUT_VERBOSE) {
		fprintf(stdout, "------------------------------------------\n");
		fprintf(stdout, "Success %d\n", 0);
		fprintf(stdout, "Fail    %d\n", 0);
		fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	}

}
