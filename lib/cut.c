/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <stdarg.h>
#include "cut.h"

static jmp_buf global_jbuf;

struct cut_assertion {
	int (*condition)();
	char *fmt;
};



static int __cut_assert_int_equal(va_list args)
{
	int a, b;

	a = va_arg(args, int);
	b = va_arg(args, int);

	return (a == b);
}

static int __cut_assert_int_not_equal(va_list args)
{
	int a, b;

	a = va_arg(args, int);
	b = va_arg(args, int);

	return (a != b);
}

static int __cut_assert_int_in_range(va_list args)
{
	int min, max, val;

	min = va_arg(args, int);
	max = va_arg(args, int);
	val = va_arg(args, int);

	return (min <= val && val <= max);
}

static int __cut_assert_int_not_in_range(va_list args)
{
	int min, max, val;

	min = va_arg(args, int);
	max = va_arg(args, int);
	val = va_arg(args, int);

	return (min > val || val > max);
}

static int __cut_assert_mem_not_null(va_list args)
{
	void *ptr;

	ptr = va_arg(args, void*);

	return (ptr != NULL);
}

static int __cut_assert_mem_null(va_list args)
{
	void *ptr;

	ptr = va_arg(args, void*);

	return (ptr == NULL);
}

/**
 * List of known test conditions
 */
static const struct cut_assertion asserts[] = {
	[CUT_INT_EQ] = {
		.condition = __cut_assert_int_equal,
		.fmt = "Expected <%d>, but got <%d>",
	},
	[CUT_INT_NEQ] = {
		.condition = __cut_assert_int_not_equal,
		.fmt = "Expected any but not <%d>, but got <%d>",
	},
	[CUT_INT_RANGE] = {
		.condition = __cut_assert_int_in_range,
		.fmt = "Expected in range [%d, %d], but got <%d>",
	},
	[CUT_INT_NRANGE] = {
		.condition = __cut_assert_int_not_in_range,
		.fmt = "Expected outside range [%d, %d], but got <%d>",
	},
	[CUT_MEM_NOT_NULL] = {
		.condition = __cut_assert_mem_not_null,
		.fmt = "Expected not NULL pointer <%p>",
	},
	[CUT_MEM_NULL] = {
		.condition = __cut_assert_mem_null,
		.fmt = "Expected NULL pointer <%p>"
	},
};


/**
 * Generic failure function
 * It prints a failure message and it jumps out from the current test function
 * in order to run the next one
 */
static void _cut_fail(enum cut_asserts type,
		      const char *func, const unsigned int line,
		      va_list args)
{
	fprintf(stdout, "FAILURE@%s():%d - ", func, line);
	vfprintf(stdout, asserts[type].fmt, args);
	fprintf(stdout, "\n");
	longjmp(global_jbuf, CUT_JUMP_ERROR);
}


void ___cut_assert(enum cut_asserts type,
		   const char *func, const unsigned int line,
		   ...)
{
	int cond;
	va_list args;

	va_start(args, line);
	cond = asserts[type].condition(args);
	va_end(args);

	if (cond)
		return;

	va_start(args, line);
	_cut_fail(type, func, line, args);
	va_end(args);
}

static void cut_test_run(struct cut_test *cut_test)
{
	cut_test->state = CUT_STATE_RUNNING;

	/* Set up environment */
	switch (setjmp(global_jbuf)) {
	case CUT_NO_JUMP:
		if (cut_test->set_up)
			cut_test->set_up(cut_test);

		/* Run the test */
		if (cut_test->test)
			cut_test->test(cut_test);

		/* Clear the environment */
		/* Success */
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
