/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "cut.h"

void _cut_assert(struct cut_test *cut_test, unsigned int condition, char *msg,
		const char *func, unsigned int line)
{
	if (!condition) {
		if (cut_test->suite->flags & CUT_VERBOSE)
			fprintf(stdout, "Test \"%s\": FAIL@%s():%d - \"%s\"\n",
				cut_test->name, func, line, msg);
		longjmp(cut_test->jbuf, CUT_JUMP_ERROR);
	}
}

void _cut_assert_int_eq(struct cut_test *cut_test, int exp, int val,
			      const char *func, unsigned int line)
{
	char msg[64];
	int cond;

	cond = (exp == val);
	snprintf(msg, 64, "Expected <%d> but got <%d>", exp, val);
	_cut_assert(cut_test, cond, msg, func, line);
}

static void cut_test_run(struct cut_test *cut_test)
{
	cut_test->state = CUT_STATE_RUNNING;

	/* Set up environment */
	switch (setjmp(cut_test->jbuf)) {
	case CUT_NO_JUMP:
		if (cut_test->set_up)
			cut_test->set_up(cut_test);

		/* Run the test */
		if (cut_test->test)
			cut_test->test(cut_test);

		/* Clear the environment */
		/* Success */
		if (cut_test->suite->flags & CUT_VERBOSE)
			fprintf(stdout, "Test \"%s\": SUCCESS\n",
				cut_test->name);
		cut_test->state = CUT_STATE_SUCCESS;
		cut_test->suite->success_count++;
		if (cut_test->tear_down)
			cut_test->tear_down(cut_test);
		break;
	case CUT_JUMP_ERROR: /* test failed */
		cut_test->state = CUT_STATE_ERROR;
		cut_test->suite->fail_count++;
		if (cut_test->tear_down)
			cut_test->tear_down(cut_test);

	case CUT_JUMP_TEAR_FAIL: /* test fine, but something is wrong */
		break;
	}
}


/**
 * It creates a test suite, in other words a collection of tests.
 *
 * @param[in] name suite name
 */
void cut_suite_init(struct cut_suite *suite)
{
	int i;

	for (i = 0; i < suite->test_count; i++) {
		suite->tests[i].state = CUT_STATE_STOP;
		suite->tests[i].suite = suite;
	}
}


/**
 * It runs all the tests within the given suite
 * @param[in] cut_suite the suite to run
 */
void cut_suite_run(struct cut_suite *cut_suite, unsigned long flags)
{
	int i;

	cut_suite->flags = flags;
	cut_suite->success_count = 0;
	cut_suite->fail_count = 0;

	if (flags & CUT_VERBOSE) {
		fprintf(stdout, "Running suite \"%s\"\n", cut_suite->name);
		fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	}

	if (cut_suite->set_up)
		cut_suite->set_up(cut_suite);
	for (i = 0; i < cut_suite->test_count; i++) {
		/* Run test */
		cut_test_run(&cut_suite->tests[i]);
	}
	if (cut_suite->tear_down)
		cut_suite->tear_down(cut_suite);

	if (flags & CUT_VERBOSE) {
		fprintf(stdout, "------------------------------------------\n");
		fprintf(stdout, "Success %d\n", cut_suite->success_count);
		fprintf(stdout, "Fail    %d\n", cut_suite->fail_count);
		fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	}

}
