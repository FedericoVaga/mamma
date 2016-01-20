/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */

#ifndef __M_H__
#define __M_H__

#include <setjmp.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

enum m_asserts {
	M_INT_EQ = 0,
	M_INT_NEQ ,
	M_INT_RANGE,
	M_INT_NRANGE,
	M_MEM_NOT_NULL,
	M_MEM_NULL,
	M_MEM_EQ,
	M_MEM_NEQ,
	__M_MAX_STANDARD_ASSERTION,
};

enum m_test_state {
	M_STATE_STOP = 0,
	M_STATE_RUNNING,
	M_STATE_ERROR,
	M_STATE_SUCCESS,
	M_STATE_SKIP,
};

enum m_test_jump_conditions {
	M_NO_JUMP = 0,
	M_JUMP_ERROR,
	M_JUMP_SKIP,
	M_JUMP_TEAR_FAIL,
};

struct m_test {
	enum m_test_state state;
	struct m_suite *suite;
	void *private;
	void (*set_up)(struct m_test *test);
	void (*test)(struct m_test *test);
	void (*tear_down)(struct m_test *test);
};
#define m_test(_up, _down, _test, _priv) {	\
		.private = (_priv),		\
			.set_up = (_up),	\
			.test = (_test),	\
			.tear_down = (_down),	\
			}


/**
 * Test suite container. It is a collection of tests
 */
struct m_suite {
	const char *name; /**< suite name */
	unsigned long flags;
	void *private;
	struct m_test *tests; /**< list of tests */
	const unsigned int test_count; /**< number of valid tests */
	void (*set_up)(struct m_suite *m_suite);
	void (*tear_down)(struct m_suite *m_suite);
	char *(*strerror)(int errnum);
	unsigned int success_count;
	unsigned int fail_count;
	unsigned int skip_count;
};
#define m_suite(_name, _test, _test_count, _up, _down, _priv) {	\
		.name = (_name),					\
			.private = (_priv),				\
			.tests = (_test),				\
			.test_count = (_test_count),			\
			.set_up = (_up),				\
			.tear_down = (_down),				\
			}

#define M_VERBOSE (1 << 0)
#define M_ERRNO (1 << 1) /**< print errno messages */

extern void m_suite_init(struct m_suite *suite);
extern void m_suite_run(struct m_suite *m_suite, unsigned long flags);

extern void _m_assert(unsigned int condition, char *msg,
			const char *func, const unsigned int line);
extern void _m_skip_test(unsigned int cond,
			   const char *func, const unsigned int line);
#define m_assert(_cond, _msg, _f, _l)	\
	_m_assert((_cond), (_msg), __func__, __LINE__)
#define m_skip(_cond) _m_skip_test((_cond), __func__, __LINE__)


/**
 * @defgroup m_assert_int Integer Assertions
 */
void ___m_assert(enum m_asserts type,
		   const char *func, const unsigned int line,
		   ...);

#define m_assert_int_eq(_exp, _val)					\
	___m_assert(M_INT_EQ, (__func__), (__LINE__), (_exp), (_val))
#define m_assert_int_neq(_exp, _val)					\
	___m_assert(M_INT_NEQ, (__func__), (__LINE__), (_exp), (_val))
#define m_assert_int_range(_min, _max, _val)				\
	___m_assert(M_INT_RANGE, (__func__), (__LINE__), (_min), (_max), (_val))
#define m_assert_int_nrange(_min, _max, _val)				\
	___m_assert(M_INT_NRANGE, (__func__), (__LINE__), (_min), (_max), (_val))
/** @} */

/**
 * @defgroup m_assert_mem Memory Assertions
 */

#define m_assert_mem_not_null(_ptr)					\
	___m_assert(M_MEM_NOT_NULL, (__func__), (__LINE__), (_ptr))
#define m_assert_mem_null(_ptr)					\
	___m_assert(M_MEM_NULL, (__func__), (__LINE__), (_ptr))
#define m_assert_mem_eq(_ptr1, _ptr2, _size)				\
	___m_assert(M_MEM_EQ, (__func__), (__LINE__), (_ptr1), (_ptr2), (_size))
#define m_assert_mem_neq(_ptr1, _ptr2, _size)				\
	___m_assert(M_MEM_NEQ, (__func__), (__LINE__), (_ptr1), (_ptr2), (_size))
/** @} */
#endif
