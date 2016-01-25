/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <stdarg.h>
#include <errno.h>
#include "mamma.h"

static jmp_buf global_jbuf;

struct m_assertion {
	int (*condition)();
	char *fmt;
};


/* -------------------------------------------------------------------- */
/* Following all implemented test conditions - comment only when needed */
/* -------------------------------------------------------------------- */

static int m_cond_int_equal(va_list args)
{
	int a, b;

	a = va_arg(args, int);
	b = va_arg(args, int);

	if (a == b) {
		return 1;
	} else {
		errno = EINVAL;
		return 0;
	}
}

static int m_cond_int_not_equal(va_list args)
{
	return !m_cond_int_equal(args);
}

static int m_cond_int_greater_than(va_list args)
{
	int a, b;

	a = va_arg(args, int);
	b = va_arg(args, int);

	if (a > b) {
		return 1;
	} else {
		errno = EINVAL;
		return 0;
	}
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

	min = va_arg(args, int);
	max = va_arg(args, int);
	val = va_arg(args, int);

	if (min <= val && val <= max) {
		return 1;
	} else {
		errno = EINVAL;
		return 0;
	}
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

	if (a == b) {
		return 1;
	} else {
		errno = EINVAL;
		return 0;
	}
}

static int m_cond_dbl_not_equal(va_list args)
{
	return !m_cond_int_equal(args);
}

static int m_cond_dbl_greater_than(va_list args)
{
	double a, b;

	a = va_arg(args, double);
	b = va_arg(args, double);

	if (a > b) {
		return 1;
	} else {
		errno = EINVAL;
		return 0;
	}
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

	if (min <= val && val <= max) {
		return 1;
	} else {
		errno = EINVAL;
		return 0;
	}
}

static int m_cond_dbl_not_in_range(va_list args)
{
	return !m_cond_dbl_in_range(args);
}



static int m_cond_ptr_null(va_list args)
{
	void *ptr;

	ptr = va_arg(args, void*);

	if (ptr == NULL) {
		return 1;
	} else {
		errno = EINVAL;
		return 0;
	}
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

	if (memcmp(ptr1, ptr2, size) == 0) {
		return 1;
	} else {
		errno = EINVAL;
		return 0;
	}
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

	if (memcmp(a, b, n) > 0) {
		return 1;
	} else {
		errno = EINVAL;
		return 0;
	}
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

	if (memcmp(a, b, n) >= 0) {
		return 1;
	} else {
		errno = EINVAL;
		return 0;
	}
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

	if (memcmp(min, val, n) <= 0 && memcmp(val, max, n) <= 0) {
		return 1;
	} else {
		errno = EINVAL;
		return 0;
	}
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

	if (memcmp(min, val, n) > 0 || memcmp(val, max, n) > 0) {
		return 1;
	} else {
		errno = EINVAL;
		return 0;
	}
}


static int m_cond_str_eq(va_list args)
{
	char *ptr1, *ptr2;

	ptr1 = va_arg(args, char*);
	ptr2 = va_arg(args, char*);

	if (strcmp(ptr1, ptr2) == 0) {
		return 1;
	} else {
		errno = EINVAL;
		return 0;
	}
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

	if (strcmp(a, b) > 0) {
		return 1;
	} else {
		errno = EINVAL;
		return 0;
	}
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

	if (strcmp(a, b) >= 0) {
		return 1;
	} else {
		errno = EINVAL;
		return 0;
	}
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

	if (strcmp(min, val) <= 0 && strcmp(val, max) <= 0) {
		return 1;
	} else {
		errno = EINVAL;
		return 0;
	}
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

	if (strcmp(min, val) > 0 || strcmp(val, max) > 0) {
		return 1;
	} else {
		errno = EINVAL;
		return 0;
	}
}
/* -------------------------------------------------------------------- */
/*                Finish of test conditions implementation              */
/* -------------------------------------------------------------------- */



/**
 * List of known test conditions
 */
static const struct m_assertion asserts[] = {
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
	[M_PTR_NOT_NULL] = {
		.condition = m_cond_ptr_not_null,
		.fmt = "Expected not NULL pointer, but got <%p>",
	},
	[M_PTR_NULL] = {
		.condition = m_cond_ptr_null,
		.fmt = "Expected NULL pointer, but got <%p>"
	},
	[M_MEM_EQ] = {
		.condition = m_cond_mem_eq,
		.fmt = "Expected the same memory content at addresses %p and %p (size: %z)"
	},
	[M_MEM_NEQ] = {
		.condition = m_cond_mem_neq,
		.fmt = "Expected different memory content at addresses %p and %p (size: %z)"
	},
	[M_MEM_GT] = {
		.condition = m_cond_mem_greater_than,
		.fmt = "Expected the memory content at addresses %p to be greater than the memory content at address %p (size: %z)",
	},
	[M_MEM_GE] = {
		.condition = m_cond_mem_greater_equal,
		.fmt = "Expected the memory content at addresses %p to be greater or equal than the memory content at address %p (size: %z)",
	},
	[M_MEM_LT] = {
		.condition = m_cond_mem_less_than,
		.fmt = "Expected the memory content at addresses %p to be less than the memory content at address %p (size: %z)",
	},
	[M_MEM_LE] = {
		.condition = m_cond_mem_less_equal,
		.fmt = "Expected the memory content at addresses %p to be less or equal than the memory content at address %p (size: %z)",
	},
	[M_MEM_RANGE] = {
		.condition = m_cond_mem_in_range,
		.fmt = "Expected the memory content at addresses %p to be within the range defined by the memory content at %p and %p (size: %z)",
	},
	[M_MEM_NRANGE] = {
		.condition = m_cond_mem_not_in_range,
		.fmt = "Expected the memory content at addresses %p to be outside the range defined by the memory content at %p and %p (size: %z)",
	},
	[M_STR_EQ] = {
		.condition = m_cond_str_eq,
		.fmt = "Expected <%s>, but got <%s>"
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
};


/**
 * Generic failure function
 * It prints a failure message and it jumps out from the current test function
 * in order to run the next one
 */
static void _m_fail(enum m_asserts type,
		      const char *func, const unsigned int line,
		      va_list args)
{
	fprintf(stdout, "FAILURE@%s():%d - ", func, line);
	vfprintf(stdout, asserts[type].fmt, args);
	fprintf(stdout, "\n");
	longjmp(global_jbuf, M_JUMP_ERROR);
}


void ___m_assert(enum m_asserts type,
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
	_m_fail(type, func, line, args);
	va_end(args);
}

void ___m_check(enum m_asserts type,
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
	fprintf(stdout, "INVALID@%s():%d - ", func, line);
	vfprintf(stdout, asserts[type].fmt, args);
	fprintf(stdout, "\n");
	va_end(args);
}

static void m_test_run(struct m_test *m_test)
{
	m_test->suite->total_count++;
	m_test->state = M_STATE_RUNNING;
	errno = 0;

	/* Set up environment */
	switch (setjmp(global_jbuf)) {
	case M_NO_JUMP:
		if (m_test->set_up)
			m_test->set_up(m_test);

		/* Run the test */
		if (m_test->test)
			m_test->test(m_test);

		/* Clear the environment */
		/* Success */
		m_test->state = M_STATE_SUCCESS;
		m_test->suite->success_count++;
		if (m_test->tear_down)
			m_test->tear_down(m_test);
		break;
	case M_JUMP_ERROR: /* test failed */
		if ((m_test->suite->flags & M_ERRNO) && errno) {
			if (m_test->suite->strerror)
				fprintf(stdout, "\tError %d: %s\n",
					errno, m_test->suite->strerror(errno));
			else
				fprintf(stdout, "\tError %d: %s\n",
					errno, strerror(errno));
		}
		m_test->state = M_STATE_ERROR;
		m_test->suite->fail_count++;

		if (m_test->tear_down)
			m_test->tear_down(m_test);
		break;
	case M_JUMP_SKIP:
		m_test->state = M_STATE_SKIP;
		m_test->suite->skip_count++;

		if (m_test->tear_down)
			m_test->tear_down(m_test);
		break;
	case M_JUMP_TEAR_FAIL: /* test fine, but something is wrong */
		break;
	}
}


void _m_skip_test(unsigned int cond, const char *func, const unsigned int line)
{
	if (!cond)
		return;
	fprintf(stdout, "SKIP@%s():%d\n", func, line);
	longjmp(global_jbuf, M_JUMP_SKIP);
}

/**
 * It creates a test suite, in other words a collection of tests.
 *
 * @param[in] name suite name
 */
void m_suite_init(struct m_suite *suite)
{
	int i;

	suite->total_count = 0;
	suite->success_count = 0;
	suite->fail_count = 0;
	suite->skip_count = 0;

	for (i = 0; i < suite->test_count; i++) {
		suite->tests[i].state = M_STATE_STOP;
		suite->tests[i].suite = suite;
	}
}


/**
 * It runs all the tests within the given suite
 * @param[in] m_suite the suite to run
 */
void m_suite_run(struct m_suite *m_suite, unsigned long flags)
{
	int i;

	m_suite->flags = flags;

	if (flags & M_VERBOSE) {
		fprintf(stdout, "Running suite \"%s\"\n", m_suite->name);
		fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	}

	if (m_suite->set_up)
		m_suite->set_up(m_suite);
	for (i = 0; i < m_suite->test_count; i++) {
		/* Run test */
		m_test_run(&m_suite->tests[i]);
	}
	if (m_suite->tear_down)
		m_suite->tear_down(m_suite);

	if (flags & M_VERBOSE) {
		fprintf(stdout, "------------------------------------------\n");
		fprintf(stdout, "Total   %d\n", m_suite->total_count);
		fprintf(stdout, "Success %d\n", m_suite->success_count);
		fprintf(stdout, "Fail    %d\n", m_suite->fail_count);
		fprintf(stdout, "Skip    %d\n", m_suite->skip_count);
		fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	}

}
