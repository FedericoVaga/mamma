/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */

#ifndef __CUT_H__
#define __CUT_H__

#include <setjmp.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

enum cut_asserts {
	CUT_INT_EQ = 0,
	CUT_INT_NEQ ,
	CUT_INT_RANGE,
	CUT_INT_NRANGE,
	CUT_MEM_NOT_NULL,
	CUT_MEM_NULL,
	CUT_MEM_EQ,
	CUT_MEM_NEQ,
	__CUT_MAX_STANDARD_ASSERTION,
};

enum cut_test_state {
	CUT_STATE_STOP = 0,
	CUT_STATE_RUNNING,
	CUT_STATE_ERROR,
	CUT_STATE_SUCCESS,
	CUT_STATE_SKIP,
};

enum cut_test_jump_conditions {
	CUT_NO_JUMP = 0,
	CUT_JUMP_ERROR,
	CUT_JUMP_SKIP,
	CUT_JUMP_TEAR_FAIL,
};

struct cut_test {
	enum cut_test_state state;
	struct cut_suite *suite;
	void *private;
	void (*set_up)(struct cut_test *test);
	void (*test)(struct cut_test *test);
	void (*tear_down)(struct cut_test *test);
};
#define cut_test(_up, _down, _test, _priv) {	\
		.private = (_priv),		\
			.set_up = (_up),	\
			.test = (_test),	\
			.tear_down = (_down),	\
			}


/**
 * Test suite container. It is a collection of tests
 */
struct cut_suite {
	const char *name; /**< suite name */
	unsigned long flags;
	void *private;
	struct cut_test *tests; /**< list of tests */
	const unsigned int test_count; /**< number of valid tests */
	void (*set_up)(struct cut_suite *cut_suite);
	void (*tear_down)(struct cut_suite *cut_suite);
	char *(*strerror)(int errnum);
	unsigned int success_count;
	unsigned int fail_count;
	unsigned int skip_count;
};
#define cut_suite(_name, _test, _test_count, _up, _down, _priv) {	\
		.name = (_name),					\
			.private = (_priv),				\
			.tests = (_test),				\
			.test_count = (_test_count),			\
			.set_up = (_up),				\
			.tear_down = (_down),				\
			}

#define CUT_VERBOSE (1 << 0)
#define CUT_ERRNO (1 << 1) /**< print errno messages */

extern void cut_suite_init(struct cut_suite *suite);
extern void cut_suite_run(struct cut_suite *cut_suite, unsigned long flags);

extern void _cut_assert(unsigned int condition, char *msg,
			const char *func, const unsigned int line);
extern void _cut_skip_test(unsigned int cond,
			   const char *func, const unsigned int line);
#define cut_assert(_cond, _msg, _f, _l)	\
	_cut_assert((_cond), (_msg), __func__, __LINE__)
#define cut_skip(_cond) _cut_skip_test((_cond), __func__, __LINE__)


/**
 * @defgroup cut_assert_int Integer Assertions
 */
void ___cut_assert(enum cut_asserts type,
		   const char *func, const unsigned int line,
		   ...);

#define cut_assert_int_eq(_exp, _val)					\
	___cut_assert(CUT_INT_EQ, (__func__), (__LINE__), (_exp), (_val))
#define cut_assert_int_neq(_exp, _val)					\
	___cut_assert(CUT_INT_NEQ, (__func__), (__LINE__), (_exp), (_val))
#define cut_assert_int_range(_min, _max, _val)				\
	___cut_assert(CUT_INT_RANGE, (__func__), (__LINE__), (_min), (_max), (_val))
#define cut_assert_int_nrange(_min, _max, _val)				\
	___cut_assert(CUT_INT_NRANGE, (__func__), (__LINE__), (_min), (_max), (_val))
/** @} */

/**
 * @defgroup cut_assert_mem Memory Assertions
 */

#define cut_assert_mem_not_null(_ptr)					\
	___cut_assert(CUT_MEM_NOT_NULL, (__func__), (__LINE__), (_ptr))
#define cut_assert_mem_null(_ptr)					\
	___cut_assert(CUT_MEM_NULL, (__func__), (__LINE__), (_ptr))
#define cut_assert_mem_eq(_ptr1, _ptr2, _size)				\
	___cut_assert(CUT_MEM_EQ, (__func__), (__LINE__), (_ptr1), (_ptr2), (_size))
#define cut_assert_mem_neq(_ptr1, _ptr2, _size)				\
	___cut_assert(CUT_MEM_NEQ, (__func__), (__LINE__), (_ptr1), (_ptr2), (_size))
/** @} */
#endif
