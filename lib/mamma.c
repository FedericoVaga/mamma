/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include "mamma.h"


/**
 * This structure represent the current status of the state machine.
 * This structure assume that there are no parallelism among tests and
 * test-suites. This means that all tests and test-suites run one after
 * the other. Thanks to this assumption we can keep track of current
 * status globally.
 */
static struct m_status {
	jmp_buf global_jbuf; /**< jump bookmark */
	enum m_state_machine state_cur; /**< current state-machine state */
	enum m_state_machine state_prv; /**< previous state-machine state */
	struct m_suite *m_suite_cur; /**< current test-suite running */
	struct m_test *m_test_cur; /**< current test running */
} status;


/**
 * Data structure describing an assertion
 */
struct m_assertion {
	int (*condition)(va_list args); /**< function that evaluate
					     the assertion condition */
	const char *fmt; /**< error's format string */
};



/* -------------------------------------------------------------------- */
/*                  Test State Machine implementation                   */
/* -------------------------------------------------------------------- */

/**
 * It does the transition between states
 * @param[in] state next state
 */
static void m_state_go_to(enum m_state_machine state)
{
	status.state_prv = status.state_cur;
	longjmp(status.global_jbuf, state);
}

/**
 * It set up the suite environment
 */
static void m_state_suite_set_up(void)
{
	if (status.m_suite_cur->set_up)
		status.m_suite_cur->set_up(status.m_suite_cur);

	status.m_test_cur = &status.m_suite_cur->tests[0];

	/* Start test execution */
	m_state_go_to(M_STATE_TEST_SET_UP);
}

/**
 * It set up the test environment
 */
static void m_state_test_set_up(void)
{
	status.m_test_cur->suite->total_count++;

	if (status.m_test_cur->set_up)
		status.m_test_cur->set_up(status.m_test_cur);

	m_state_go_to(M_STATE_TEST_RUN);
}

/**
 * It runs the test procedure
 */
static void m_state_test_run(void)
{
	if (status.m_test_cur->suite->flags & M_VERBOSE) {
		fprintf(stdout, "Suite: %s, Test: %d, Iterations: %d ...\n",
			status.m_test_cur->suite->name,
			status.m_test_cur->index,
			status.m_test_cur->loop);
		if (status.m_test_cur->desc) {
			fprintf(stdout, "%s\n", status.m_test_cur->desc);
		}
		fprintf(stdout, "\n");
	}
	if (status.m_test_cur->test) {
		int i;

		for (i = 0; i < status.m_test_cur->loop; ++i) {
			if (status.m_test_cur->suite->flags & M_VERBOSE)
				fprintf(stdout, ".");
			status.m_test_cur->test(status.m_test_cur);
		}
	}
	if (status.m_test_cur->suite->flags & M_VERBOSE)
		fprintf(stdout, "\n[Success]\n\n");

	status.m_test_cur->exit = M_STATE_EXIT_SUCCESS;
	status.m_test_cur->suite->success_count++;

	m_state_go_to(M_STATE_TEST_TEAR_DOWN);
}

/**
 * It undo what m_state_test_set_up() did
 */
static void m_state_test_tear_down(void)
{
	if (status.m_test_cur->tear_down)
		status.m_test_cur->tear_down(status.m_test_cur);

	m_state_go_to(M_STATE_TEST_EXIT);
}

/**
 * It handles error
 */
static void m_state_test_error_skip(void)
{
	status.m_test_cur->exit = M_STATE_EXIT_ERROR;
	switch (status.state_prv) {
	case M_STATE_SUITE_SET_UP:
		m_state_go_to(M_STATE_SUITE_TEAR_DOWN);
	case M_STATE_TEST_SET_UP:
	case M_STATE_TEST_RUN:
		/*
		 * We can jump to TEAR_DOWN state only if the state that
		 * raised the error was SET_UP or RUN. Any other case is
		 * wrong (ERROR, SKIP, EXIT) or is not possible to recover
		 * (TEAR_DOWN, it while loop forever)
		 */
		switch (status.state_cur) {
		case M_STATE_TEST_ERROR:
			status.m_test_cur->suite->fail_count++;
			break;
		case M_STATE_TEST_SKIP:
			status.m_test_cur->exit = M_STATE_EXIT_SKIP;
			status.m_test_cur->suite->skip_count++;
			break;
		default:
			/* Should not happen */
			assert(0);
		}
		m_state_go_to(M_STATE_TEST_TEAR_DOWN);
	case M_STATE_SUITE_TEAR_DOWN:
	case M_STATE_TEST_TEAR_DOWN:
		m_state_go_to(M_STATE_TEST_EXIT);
	default:
		/* Should not happen */
		assert(0);
	}
}

/**
 * It complete the test execution
 */
static void m_state_test_exit(void)
{
	if (status.m_test_cur->index + 1 < status.m_suite_cur->test_count) {
		status.m_test_cur = &status.m_suite_cur->tests[status.m_test_cur->index + 1];
		m_state_go_to(M_STATE_TEST_SET_UP);
	} else {
		m_state_go_to(M_STATE_SUITE_TEAR_DOWN);
	}
}

/**
 * It undo what m_state_test_set_up() did
 */
static void m_state_suite_tear_down(void)
{
	if (status.m_suite_cur->tear_down)
		status.m_suite_cur->tear_down(status.m_suite_cur);

	m_state_go_to(M_STATE_SUITE_EXIT);
}

/**
 * It undo what m_state_test_set_up() did
 */
static void m_state_suite_exit(void)
{
	/* Nothing to do, this is the exit point */
}


/**
 * List of all possible states of the state-machine
 */
static void (*state_machine[_M_STATE_MAX])(void) = {
	[M_STATE_SUITE_SET_UP] = m_state_suite_set_up,
	[M_STATE_TEST_SET_UP] = m_state_test_set_up,
	[M_STATE_TEST_RUN] = m_state_test_run,
	[M_STATE_TEST_TEAR_DOWN] = m_state_test_tear_down,
	[M_STATE_TEST_ERROR] = m_state_test_error_skip,
	[M_STATE_TEST_SKIP] = m_state_test_error_skip,
	[M_STATE_TEST_EXIT] = m_state_test_exit,
	[M_STATE_SUITE_TEAR_DOWN] = m_state_suite_tear_down,
	[M_STATE_SUITE_EXIT] = m_state_suite_exit,
};


/**
 * It rungs the given test.
 * Running a test means activate the test state machine. This state machine is
 * based on function callback and long-jump. Each state callback knows which are
 * the next possible state and it uses a long-jump to go to the next state.
 * I'm using the callback way because I think is better and cleaner than the
 * switch way. I'm using long-jump to go between states because it is necessary
 * to jump out from error condition, so instead of having transition system I
 * prefer to have an unifor way to go between states
 * @param[in] m_test mamma's test to execute
 */
static void m_suite_run_state_machine(struct m_suite *m_suite)
{
	status.m_suite_cur = m_suite;
	errno = 0;

	status.state_cur = setjmp(status.global_jbuf);
	assert(status.state_cur < _M_STATE_MAX);
	state_machine[status.state_cur]();
}


/* -------------------------------------------------------------------- */
/* Following all implemented test conditions - comment only when needed */
/* -------------------------------------------------------------------- */

static int m_cond_true(va_list args)
{
	return !!va_arg(args, int);
}

static int m_cond_false(va_list args)
{
	return !m_cond_true(args);
}



static int m_cond_int_equal(va_list args)
{
	int a, b;

	a = va_arg(args, long);
	b = va_arg(args, long);

	return (a == b);
}

static int m_cond_int_not_equal(va_list args)
{
	return !m_cond_int_equal(args);
}

static int m_cond_int_greater_than(va_list args)
{
	int a, b;

	a = va_arg(args, long);
	b = va_arg(args, long);

	return (a > b);
}

static int m_cond_int_greater_equal(va_list args)
{
	va_list args_bis;
	int cond;

	va_copy(args_bis, args);
	cond = (m_cond_int_greater_than(args) || m_cond_int_equal(args_bis));
	va_end(args_bis);

	return cond;
}

static int m_cond_int_less_than(va_list args)
{
	return !m_cond_int_greater_equal(args);
}

static int m_cond_int_less_equal(va_list args)
{
	return !m_cond_int_greater_than(args);
}

static int m_cond_int_in_range(va_list args)
{
	int min, max, val;

	min = va_arg(args, long);
	max = va_arg(args, long);
	val = va_arg(args, long);

	return (min <= val && val <= max);
}

static int m_cond_int_not_in_range(va_list args)
{
	return !m_cond_int_in_range(args);
}



static int m_cond_dbl_equal(va_list args)
{
	double a, b;

	a = va_arg(args, double);
	b = va_arg(args, double);

	return (a == b);
}

static int m_cond_dbl_not_equal(va_list args)
{
	return !m_cond_dbl_equal(args);
}

static int m_cond_dbl_greater_than(va_list args)
{
	double a, b;

	a = va_arg(args, double);
	b = va_arg(args, double);

	return (a > b);
}

static int m_cond_dbl_greater_equal(va_list args)
{
	va_list args_bis;
	int cond;

	va_copy(args_bis, args);
	cond = (m_cond_dbl_greater_than(args) || m_cond_dbl_equal(args_bis));
	va_end(args_bis);

	return cond;
}

static int m_cond_dbl_less_than(va_list args)
{
	return !m_cond_dbl_greater_equal(args);
}

static int m_cond_dbl_less_equal(va_list args)
{
	return !m_cond_dbl_greater_than(args);
}

static int m_cond_dbl_in_range(va_list args)
{
	double min, max, val;

	min = va_arg(args, double);
	max = va_arg(args, double);
	val = va_arg(args, double);

	return (min <= val && val <= max);
}

static int m_cond_dbl_not_in_range(va_list args)
{
	return !m_cond_dbl_in_range(args);
}



static int m_cond_ptr_null(va_list args)
{
	void *ptr;

	ptr = va_arg(args, void*);

	return (ptr == NULL);
}

static int m_cond_ptr_not_null(va_list args)
{
	return !m_cond_ptr_null(args);
}


static int m_cond_mem_eq(va_list args)
{
	void *ptr1, *ptr2;
	size_t size;

	ptr1 = va_arg(args, void*);
	ptr2 = va_arg(args, void*);
	size = va_arg(args, size_t);

	return (memcmp(ptr1, ptr2, size) == 0);
}

static int m_cond_mem_neq(va_list args)
{
	return !m_cond_mem_eq(args);
}

static int m_cond_mem_greater_than(va_list args)
{
	void *a, *b;
	size_t n;

	a = va_arg(args, void*);
	b = va_arg(args, void*);
	n = va_arg(args, size_t);

	return (memcmp(a, b, n) > 0);
}


/**
 * Implement the function instead of re-use the others (like we did for
 * int and double) in order to avoid double memory scanning
 */
static int m_cond_mem_greater_equal(va_list args)
{
	void *a, *b;
	size_t n;

	a = va_arg(args, void*);
	b = va_arg(args, void*);
	n = va_arg(args, size_t);

	return (memcmp(a, b, n) >= 0);
}

static int m_cond_mem_less_than(va_list args)
{
	return !m_cond_mem_greater_equal(args);
}

static int m_cond_mem_less_equal(va_list args)
{
	return !m_cond_mem_greater_than(args);
}

static int m_cond_mem_in_range(va_list args)
{
	void *min, *max, *val;
	size_t n;

	min = va_arg(args, void*);
	max = va_arg(args, void*);
	val = va_arg(args, void*);
	n = va_arg(args, size_t);

	return (memcmp(min, val, n) <= 0 && memcmp(val, max, n) <= 0);
}

/**
 * Implement the function instead of re-use the others (like we did for
 * int and double) in order to avoid double memory scanning
 */
static int m_cond_mem_not_in_range(va_list args)
{
	void *min, *max, *val;
	size_t n;

	min = va_arg(args, void*);
	max = va_arg(args, void*);
	val = va_arg(args, void*);
	n = va_arg(args, size_t);

	return (memcmp(min, val, n) > 0 || memcmp(val, max, n) > 0);
}


static int m_cond_str_eq(va_list args)
{
	char *ptr1, *ptr2;

	ptr1 = va_arg(args, char*);
	ptr2 = va_arg(args, char*);

	return (strcmp(ptr1, ptr2) == 0);
}

static int m_cond_str_neq(va_list args)
{
	return !m_cond_str_eq(args);
}

static int m_cond_str_greater_than(va_list args)
{
	char *a, *b;

	a = va_arg(args, char*);
	b = va_arg(args, char*);

	return (strcmp(a, b) > 0);
}


/**
 * Implement the function instead of re-use the others (like we did for
 * int and double) in order to avoid double strory scanning
 */
static int m_cond_str_greater_equal(va_list args)
{
	char *a, *b;

	a = va_arg(args, char*);
	b = va_arg(args, char*);

	return (strcmp(a, b) >= 0);
}

static int m_cond_str_less_than(va_list args)
{
	return !m_cond_str_greater_equal(args);
}

static int m_cond_str_less_equal(va_list args)
{
	return !m_cond_str_greater_than(args);
}

static int m_cond_str_in_range(va_list args)
{
	char *min, *max, *val;

	min = va_arg(args, char*);
	max = va_arg(args, char*);
	val = va_arg(args, char*);

	return (strcmp(min, val) <= 0 && strcmp(val, max) <= 0);
}

/**
 * Implement the function instead of re-use the others (like we did for
 * int and double) in order to avoid double string scanning
 */
static int m_cond_str_not_in_range(va_list args)
{
	char *min, *max, *val;

	min = va_arg(args, char*);
	max = va_arg(args, char*);
	val = va_arg(args, char*);

	return (strcmp(min, val) > 0 || strcmp(val, max) > 0);
}


/* -------------------------------------------------------------------- */
/*                Finish of test conditions implementation              */
/* -------------------------------------------------------------------- */



/**
 * List of known test conditions
 */
static const struct m_assertion asserts[] = {
	[M_CUSTOM] = { /* Handled directly by m_check() */
		.condition = NULL,
		.fmt = NULL,
	},
	/* Boolean */
	[M_TRUE] = {
		.condition = m_cond_true,
		.fmt = "Expected \"True\" condition but got \"False\"",
	},
	[M_FALSE] = {
		.condition = m_cond_false,
		.fmt = "Expected \"False\" condition but got \"True\"",
	},
	/* Integer */
	[M_INT_EQ] = {
		.condition = m_cond_int_equal,
		.fmt = "Expected <%d>, but got <%d>",
	},
	[M_INT_NEQ] = {
		.condition = m_cond_int_not_equal,
		.fmt = "Expected any but not <%d>, but got <%d>",
	},
	[M_INT_RANGE] = {
		.condition = m_cond_int_in_range,
		.fmt = "Expected in range [%d, %d], but got <%d>",
	},
	[M_INT_NRANGE] = {
		.condition = m_cond_int_not_in_range,
		.fmt = "Expected outside range [%d, %d], but got <%d>",
	},
	[M_INT_GT] = {
		.condition = m_cond_int_greater_than,
		.fmt = "Expected <%d> greater than <%d>",
	},
	[M_INT_GE] = {
		.condition = m_cond_int_greater_equal,
		.fmt = "Expected <%d> greater or equal than <%d>",
	},
	[M_INT_LT] = {
		.condition = m_cond_int_less_than,
		.fmt = "Expected <%d> less than <%d>",
	},
	[M_INT_LE] = {
		.condition = m_cond_int_less_equal,
		.fmt = "Expected <%d> less or equal than <%d>",
	},
	/* Floating Point */
	[M_DBL_EQ] = {
		.condition = m_cond_dbl_equal,
		.fmt = "Expected <%f>, but got <%f>",
	},
	[M_DBL_NEQ] = {
		.condition = m_cond_dbl_not_equal,
		.fmt = "Expected any but not <%f>, but got <%f>",
	},
	[M_DBL_RANGE] = {
		.condition = m_cond_dbl_in_range,
		.fmt = "Expected in range [%f, %f], but got <%f>",
	},
	[M_DBL_NRANGE] = {
		.condition = m_cond_dbl_not_in_range,
		.fmt = "Expected outside range [%f, %f], but got <%f>",
	},
	[M_DBL_GT] = {
		.condition = m_cond_dbl_greater_than,
		.fmt = "Expected <%f> greater than <%f>",
	},
	[M_DBL_GE] = {
		.condition = m_cond_dbl_greater_equal,
		.fmt = "Expected <%f> greater or equal than <%f>",
	},
	[M_DBL_LT] = {
		.condition = m_cond_dbl_less_than,
		.fmt = "Expected <%f> less than <%f>",
	},
	[M_DBL_LE] = {
		.condition = m_cond_dbl_less_equal,
		.fmt = "Expected <%f> less or equal than <%f>",
	},
	/* Memory Pointer */
	[M_PTR_NOT_NULL] = {
		.condition = m_cond_ptr_not_null,
		.fmt = "Expected not NULL pointer, but got <%p>",
	},
	[M_PTR_NULL] = {
		.condition = m_cond_ptr_null,
		.fmt = "Expected NULL pointer, but got <%p>",
	},
	/* Memory Area */
	[M_MEM_EQ] = {
		.condition = m_cond_mem_eq,
		.fmt = "Expected the same memory content at addresses %p and %p (size: %zd)",
	},
	[M_MEM_NEQ] = {
		.condition = m_cond_mem_neq,
		.fmt = "Expected different memory content at addresses %p and %p (size: %zd)",
	},
	[M_MEM_GT] = {
		.condition = m_cond_mem_greater_than,
		.fmt = "Expected the memory content at addresses %p to be greater than the memory content at address %p (size: %zd)",
	},
	[M_MEM_GE] = {
		.condition = m_cond_mem_greater_equal,
		.fmt = "Expected the memory content at addresses %p to be greater or equal than the memory content at address %p (size: %zd)",
	},
	[M_MEM_LT] = {
		.condition = m_cond_mem_less_than,
		.fmt = "Expected the memory content at addresses %p to be less than the memory content at address %p (size: %zd)",
	},
	[M_MEM_LE] = {
		.condition = m_cond_mem_less_equal,
		.fmt = "Expected the memory content at addresses %p to be less or equal than the memory content at address %p (size: %zd)",
	},
	[M_MEM_RANGE] = {
		.condition = m_cond_mem_in_range,
		.fmt = "Expected the memory content at addresses %p to be within the range defined by the memory content at %p and %p (size: %zd)",
	},
	[M_MEM_NRANGE] = {
		.condition = m_cond_mem_not_in_range,
		.fmt = "Expected the memory content at addresses %p to be outside the range defined by the memory content at %p and %p (size: %zd)",
	},
	/* String */
	[M_STR_EQ] = {
		.condition = m_cond_str_eq,
		.fmt = "Expected <%s>, but got <%s>",
	},
	[M_STR_NEQ] = {
		.condition = m_cond_str_neq,
		.fmt = "Expected any but not <%s>, but got <%s>",
	},
	[M_STR_GT] = {
		.condition = m_cond_str_greater_than,
		.fmt = "Expected <%s> greater than <%s> (ASCII order)",
	},
	[M_STR_GE] = {
		.condition = m_cond_str_greater_equal,
		.fmt = "Expected <%s> greater or equal than <%s> (ASCII order)",
	},
	[M_STR_LT] = {
		.condition = m_cond_str_less_than,
		.fmt = "Expected <%s> less than <%s> (ASCII order)",
	},
	[M_STR_LE] = {
		.condition = m_cond_str_less_equal,
		.fmt = "Expected <%s> less or equal than <%s> (ASCII order)",
	},
	[M_STR_RANGE] = {
		.condition = m_cond_str_in_range,
		.fmt = "Expected in range [%s, %s], but got <%s> (ASCII order)",
	},
	[M_STR_NRANGE] = {
		.condition = m_cond_str_not_in_range,
		.fmt = "Expected outside range [%s, %s], but got <%s> (ASCII order)",
	},

	/* Errno */
	[M_ERR_EQ] = {
		.condition = m_cond_int_equal,
		.fmt = "Expected errno <%d>, but got <%d>",
	},
	[M_ERR_NEQ] = {
		.condition = m_cond_int_not_equal,
		.fmt = "Expected any but not errno <%d>, but got <%d>",
	},
};


/**
 * It prints on stdout the given error message
 * @param[in] type type of assertion
 * @param[in] fmt printf string format
 * @param[in] func function name that called this function
 * @param[in] line source code line where this function has being called
 * @param[in] args printf parameters
 */
static void  m_print_test_msg(enum m_asserts type, const char *fmt,
			      const char *func, const unsigned int line,
			      va_list args)
{
	struct m_suite *suite = status.m_test_cur->suite;

	/* print the error if there is a valid printf format */
	if (!fmt)
		return;

	if (type == M_CUSTOM) {
		/* Skip first parameters */
		va_arg(args, int);
		va_arg(args, int);
		va_arg(args, char*);
	}
	fprintf(stdout, "ERROR @ %s():%d - ", func, line);
	vfprintf(stdout, fmt, args);
	if ((suite->flags & M_ERRNO_FUNC) &&
	    (type == M_ERR_EQ || type == M_ERR_NEQ))
		fprintf(stdout, ": %s", suite->strerror(errno));
	fprintf(stdout, "\n");
}


/**
 * Predefined check function.
 * @param[in] type type of assertion
 * @param[in] flags check options
 * @param[in] func function name that called this function
 * @param[in] line source code line where this function has being called
 */
void m_check(enum m_asserts type, unsigned long flags,
	     const char *func, const unsigned int line,
	     ...)
{
	va_list args;
	const char *fmt;
	int cond;

	/* Check condition and get print arguments */
	va_start(args, line);
	if (type == M_CUSTOM) {
		/* When custum, get all assertion data from variadic */
		cond = va_arg(args, int);
		fmt = va_arg(args, char*);
	} else {
		/* Otherwas get assertion data from the table */
		cond = asserts[type].condition(args);
		fmt = asserts[type].fmt;
	}
	va_end(args);

	if (cond)
		return; /* Condition satisfied */

	/* Print the error message */
	va_start(args, line);
	m_print_test_msg(type, fmt, func, line, args);
	va_end(args);

	/* According to the given flag, continue test execution or jump */
	if (flags & M_FLAG_STOP_ON_ERROR) {
		fprintf(stdout, "  Stop test \"%s\"\n", func);
		m_state_go_to(M_STATE_TEST_ERROR);
	} else {
		fprintf(stdout, "  Continue test \"%s\" anyway\n", func);
		status.m_test_cur->warnings++;
	}
}

/**
 * It skips the current running test if the given condition is true
 * @param[in] cond condition to evaluate
 * @param[in] func function name that called this function
 * @param[in] line source code line where this function has being called
 */
void m_skip_test(unsigned int cond, const char *func, const unsigned int line)
{
	if (!cond)
		return;
	fprintf(stdout, "SKIP@%s():%d\n", func, line);

	m_state_go_to(M_STATE_TEST_SKIP);
}



/* -------------------------------------------------------------------- */
/*                        Test Suite Management                         */
/* -------------------------------------------------------------------- */

/**
 * It initialize a given test suite.
 * It resets all counters, and properly configure the tests in order
 * to be executed within the suite
 * @param[in] suite test suite to initialize
 */
static void m_suite_init(struct m_suite *suite)
{
	int i;

	status.m_suite_cur = suite;
	status.m_suite_cur->total_count = 0;
	status.m_suite_cur->success_count = 0;
	status.m_suite_cur->fail_count = 0;
	status.m_suite_cur->skip_count = 0;
	status.m_suite_cur->private = NULL;
	if (!status.m_suite_cur->strerror)
		status.m_suite_cur->strerror = strerror;

	for (i = 0; i < suite->test_count; i++) {
		status.m_suite_cur->tests[i].private = NULL;
		status.m_suite_cur->tests[i].index = i;
		status.m_suite_cur->tests[i].suite = status.m_suite_cur;
		status.m_suite_cur->tests[i].exit = M_STATE_EXIT_NORUN;
		status.m_suite_cur->tests[i].warnings = 0;
	}
}


/**
 * It prints a summary of all executed test.
 * @param[in] m_suite the suite that we are interested in
 */
static void m_suite_summary(struct m_suite *m_suite)
{
	fprintf(stdout, "Success     Fail    Skip  |   Total\n");
	fprintf(stdout, "% 7d  % 7d % 7d  | % 7d\n",
		m_suite->success_count,
		m_suite->fail_count,
		m_suite->skip_count,
		m_suite->total_count);

	/* Be sure that the state machine is working */
	assert(m_suite->success_count +
	       m_suite->fail_count +
	       m_suite->skip_count == m_suite->total_count);
}


/**
 * It runs all the tests within the given suite
 * @param[in] m_suite the suite to run
 */
void m_suite_run(struct m_suite *m_suite)
{
	m_suite_init(m_suite);

	if (m_suite->flags & M_VERBOSE) {
		fprintf(stdout, "Running suite \"%s\"\n", m_suite->name);
		fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		if (m_suite->desc) {
			fprintf(stdout, "%s\n", m_suite->desc);
			fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		}
	}

	m_suite_run_state_machine(m_suite);

	if (m_suite->flags & M_VERBOSE) {
		fprintf(stdout, "------------------------------------------\n");
		m_suite_summary(m_suite);
		fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	}
}


/**
 * It runs a single test.
 * The framework is based on test-suite, so internally it runs the
 * given test in a dummy test-suite.
 * @param[in] m_test test to run
 */
void m_test_run(struct m_test *m_test)
{
	struct m_suite m_suite;

	memset(&m_suite, 0, sizeof(struct m_suite));
	m_suite.tests = m_test;
	m_suite.test_count = 1;

	m_suite_run(&m_suite);
}
