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
	M_INT_GT,
	M_INT_GE,
	M_INT_LT,
	M_INT_LE,
	M_DBL_EQ,
	M_DBL_NEQ ,
	M_DBL_RANGE,
	M_DBL_NRANGE,
	M_DBL_GT,
	M_DBL_GE,
	M_DBL_LT,
	M_DBL_LE,
	M_STR_EQ,
	M_STR_NEQ ,
	M_STR_RANGE,
	M_STR_NRANGE,
	M_STR_GT,
	M_STR_GE,
	M_STR_LT,
	M_STR_LE,
	M_PTR_NOT_NULL,
	M_PTR_NULL,
	M_MEM_EQ,
	M_MEM_NEQ,
	M_MEM_RANGE,
	M_MEM_NRANGE,
	M_MEM_GT,
	M_MEM_GE,
	M_MEM_LT,
	M_MEM_LE,
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
	unsigned int total_count;
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
extern void ___m_assert(enum m_asserts type,
			const char *func, const unsigned int line,
			...);
extern void ___m_check(enum m_asserts type,
		       const char *func, const unsigned int line,
		       ...);

#define m_assert_int_eq(_exp, _val)					\
	___m_assert(M_INT_EQ, (__func__), (__LINE__), (_exp), (_val))
#define m_assert_int_neq(_exp, _val)					\
	___m_assert(M_INT_NEQ, (__func__), (__LINE__), (_exp), (_val))
#define m_assert_int_gt(_exp, _val)					\
	___m_assert(M_INT_GT, (__func__), (__LINE__), (_exp), (_val))
#define m_assert_int_ge(_exp, _val)					\
	___m_assert(M_INT_GE, (__func__), (__LINE__), (_exp), (_val))
#define m_assert_int_lt(_exp, _val)					\
	___m_assert(M_INT_LT, (__func__), (__LINE__), (_exp), (_val))
#define m_assert_int_le(_exp, _val)					\
	___m_assert(M_INT_LE, (__func__), (__LINE__), (_exp), (_val))
#define m_assert_int_range(_min, _max, _val)				\
	___m_assert(M_INT_RANGE, (__func__), (__LINE__), (_min), (_max), (_val))
#define m_assert_int_nrange(_min, _max, _val)				\
	___m_assert(M_INT_NRANGE, (__func__), (__LINE__), (_min), (_max), (_val))

#define m_check_int_eq(_exp, _val)					\
	___m_check(M_INT_EQ, (__func__), (__LINE__), (_exp), (_val))
#define m_check_int_neq(_exp, _val)					\
	___m_check(M_INT_NEQ, (__func__), (__LINE__), (_exp), (_val))
#define m_check_int_gt(_exp, _val)					\
	___m_check(M_INT_GT, (__func__), (__LINE__), (_exp), (_val))
#define m_check_int_ge(_exp, _val)					\
	___m_check(M_INT_GE, (__func__), (__LINE__), (_exp), (_val))
#define m_check_int_lt(_exp, _val)					\
	___m_check(M_INT_LT, (__func__), (__LINE__), (_exp), (_val))
#define m_check_int_le(_exp, _val)					\
	___m_check(M_INT_LE, (__func__), (__LINE__), (_exp), (_val))
#define m_check_int_range(_min, _max, _val)				\
	___m_check(M_INT_RANGE, (__func__), (__LINE__), (_min), (_max), (_val))
#define m_check_int_nrange(_min, _max, _val)				\
	___m_check(M_INT_NRANGE, (__func__), (__LINE__), (_min), (_max), (_val))
/** @} */
/**
 * @defgroup m_assert_dbl Float Assertions
 */
#define m_assert_dbl_eq(_exp, _val)					\
	___m_assert(M_DBL_EQ, (__func__), (__LINE__), (double)(_exp), (double)(_val))
#define m_assert_dbl_neq(_exp, _val)					\
	___m_assert(M_DBL_NEQ, (__func__), (__LINE__), (double)(_exp), (double)(_val))
#define m_assert_dbl_gt(_exp, _val)					\
	___m_assert(M_DBL_GT, (__func__), (__LINE__), (double)(_exp), (double)(_val))
#define m_assert_dbl_ge(_exp, _val)					\
	___m_assert(M_DBL_GE, (__func__), (__LINE__), (double)(_exp), (double)(_val))
#define m_assert_dbl_lt(_exp, _val)					\
	___m_assert(M_DBL_LT, (__func__), (__LINE__), (double)(_exp), (double)(_val))
#define m_assert_dbl_le(_exp, _val)					\
	___m_assert(M_DBL_LE, (__func__), (__LINE__), (double)(_exp), (double)(_val))
#define m_assert_dbl_range(_min, _max, _val)				\
	___m_assert(M_DBL_RANGE, (__func__), (__LINE__), (double)(_min), (double)(_max), (double)(_val))
#define m_assert_dbl_nrange(_min, _max, _val)				\
	___m_assert(M_DBL_NRANGE, (__func__), (__LINE__), (double)(_min), (double)(_max), (double)(_val))

#define m_check_dbl_eq(_exp, _val)					\
	___m_check(M_DBL_EQ, (__func__), (__LINE__), (double)(_exp), (double)(_val))
#define m_check_dbl_neq(_exp, _val)					\
	___m_check(M_DBL_NEQ, (__func__), (__LINE__), (double)(_exp), (double)(_val))
#define m_check_dbl_gt(_exp, _val)					\
	___m_check(M_DBL_GT, (__func__), (__LINE__), (double)(_exp), (double)(_val))
#define m_check_dbl_ge(_exp, _val)					\
	___m_check(M_DBL_GE, (__func__), (__LINE__), (double)(_exp), (double)(_val))
#define m_check_dbl_lt(_exp, _val)					\
	___m_check(M_DBL_LT, (__func__), (__LINE__), (double)(_exp), (double)(_val))
#define m_check_dbl_le(_exp, _val)					\
	___m_check(M_DBL_LE, (__func__), (__LINE__), (double)(_exp), (double)(_val))
#define m_check_dbl_range(_min, _max, _val)				\
	___m_check(M_DBL_RANGE, (__func__), (__LINE__), (double)(_min), (double)(_max), (double)(_val))
#define m_check_dbl_nrange(_min, _max, _val)				\
	___m_check(M_DBL_NRANGE, (__func__), (__LINE__), (double)(_min), (double)(_max), (double)(_val))
 /** @} */

/**
 * @defgroup m_assert_mem Memory Assertions
 */
#define m_assert_mem_not_null(_ptr)					\
	___m_assert(M_PTR_NOT_NULL, (__func__), (__LINE__), (void *)(_ptr))
#define m_assert_mem_null(_ptr)					\
	___m_assert(M_PTR_NULL, (__func__), (__LINE__), (void *)(_ptr))
#define m_assert_mem_eq(_ptr1, _ptr2, _size)				\
	___m_assert(M_MEM_EQ, (__func__), (__LINE__), (void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_assert_mem_neq(_ptr1, _ptr2, _size)				\
	___m_assert(M_MEM_NEQ, (__func__), (__LINE__), (void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_assert_mem_gt(_ptr1, _ptr2, _size)				\
	___m_assert(M_MEM_GT, (__func__), (__LINE__), (void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_assert_mem_ge(_ptr1, _ptr2, _size)				\
	___m_assert(M_MEM_GE, (__func__), (__LINE__), (void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_assert_mem_lt(_ptr1, _ptr2, _size)				\
	___m_assert(M_MEM_LT, (__func__), (__LINE__), (void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_assert_mem_le(_ptr1, _ptr2, _size)				\
	___m_assert(M_MEM_LE, (__func__), (__LINE__), (void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_assert_mem_range(_ptr1, _ptr2, _ptr3, _size)			\
	___m_assert(M_MEM_RANGE, (__func__), (__LINE__), (void *)(_ptr1), (void *)(_ptr2), (void *)(_ptr3), (size_t)(_size))
#define m_assert_mem_nrange(_ptr1, _ptr2, _ptr3, _size)			\
	___m_assert(M_MEM_NRANGE, (__func__), (__LINE__), (void *)(_ptr1), (void *)(_ptr2), (void *)(_ptr3), (size_t)(_size))

#define m_check_mem_not_null(_ptr)					\
	___m_check(M_PTR_NOT_NULL, (__func__), (__LINE__), (void *)(_ptr))
#define m_check_mem_null(_ptr)					\
	___m_check(M_PTR_NULL, (__func__), (__LINE__), (void *)(_ptr))
#define m_check_mem_eq(_ptr1, _ptr2, _size)				\
	___m_check(M_MEM_EQ, (__func__), (__LINE__), (void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_check_mem_neq(_ptr1, _ptr2, _size)				\
	___m_check(M_MEM_NEQ, (__func__), (__LINE__), (void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_check_mem_gt(_ptr1, _ptr2, _size)				\
	___m_check(M_MEM_GT, (__func__), (__LINE__), (void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_check_mem_ge(_ptr1, _ptr2, _size)				\
	___m_check(M_MEM_GE, (__func__), (__LINE__), (void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_check_mem_lt(_ptr1, _ptr2, _size)				\
	___m_check(M_MEM_LT, (__func__), (__LINE__), (void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_check_mem_le(_ptr1, _ptr2, _size)				\
	___m_check(M_MEM_LE, (__func__), (__LINE__), (void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_check_mem_range(_ptr1, _ptr2, _ptr3, _size)			\
	___m_check(M_MEM_RANGE, (__func__), (__LINE__), (void *)(_ptr1), (void *)(_ptr2), (void *)(_ptr3), (size_t)(_size))
#define m_check_mem_nrange(_ptr1, _ptr2, _ptr3, _size)			\
	___m_check(M_MEM_NRANGE, (__func__), (__LINE__), (void *)(_ptr1), (void *)(_ptr2), (void *)(_ptr3), (size_t)(_size))
/** @} */


/**
 * @defgroup m_assert_str String Assertions
 */
#define m_assert_str_not_null(_ptr) m_assert_mem_not_null((_ptr))
#define m_assert_str_null(_ptr) m_assert_mem_null((_ptr))
#define m_assert_str_eq(_ptr1, _ptr2, _size)				\
	___m_assert(M_STR_EQ, (__func__), (__LINE__), (char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_assert_str_neq(_ptr1, _ptr2, _size)				\
	___m_assert(M_STR_NEQ, (__func__), (__LINE__), (char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_assert_str_gt(_ptr1, _ptr2, _size)				\
	___m_assert(M_STR_GT, (__func__), (__LINE__), (char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_assert_str_ge(_ptr1, _ptr2, _size)				\
	___m_assert(M_STR_GE, (__func__), (__LINE__), (char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_assert_str_lt(_ptr1, _ptr2, _size)				\
	___m_assert(M_STR_LT, (__func__), (__LINE__), (char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_assert_str_le(_ptr1, _ptr2, _size)				\
	___m_assert(M_STR_LE, (__func__), (__LINE__), (char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_assert_str_range(_ptr1, _ptr2, _ptr3, _size)			\
	___m_assert(M_STR_RANGE, (__func__), (__LINE__), (char *)(_ptr1), (char *)(_ptr2), (char *)(_ptr3), (size_t)(_size))
#define m_assert_str_nrange(_ptr1, _ptr2, _ptr3, _size)			\
	___m_assert(M_STR_NRANGE, (__func__), (__LINE__), (char *)(_ptr1), (char *)(_ptr2), (char *)(_ptr3), (size_t)(_size))

#define m_check_str_not_null(_ptr)					\
	___m_check(M_PTR_NOT_NULL, (__func__), (__LINE__), (char *)(_ptr))
#define m_check_str_null(_ptr)					\
	___m_check(M_PTR_NULL, (__func__), (__LINE__), (char *)(_ptr))
#define m_check_str_eq(_ptr1, _ptr2, _size)				\
	___m_check(M_STR_EQ, (__func__), (__LINE__), (char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_check_str_neq(_ptr1, _ptr2, _size)				\
	___m_check(M_STR_NEQ, (__func__), (__LINE__), (char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_check_str_gt(_ptr1, _ptr2, _size)				\
	___m_check(M_STR_GT, (__func__), (__LINE__), (char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_check_str_ge(_ptr1, _ptr2, _size)				\
	___m_check(M_STR_GE, (__func__), (__LINE__), (char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_check_str_lt(_ptr1, _ptr2, _size)				\
	___m_check(M_STR_LT, (__func__), (__LINE__), (char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_check_str_le(_ptr1, _ptr2, _size)				\
	___m_check(M_STR_LE, (__func__), (__LINE__), (char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_check_str_range(_ptr1, _ptr2, _ptr3, _size)			\
	___m_check(M_STR_RANGE, (__func__), (__LINE__), (char *)(_ptr1), (char *)(_ptr2), (char *)(_ptr3), (size_t)(_size))
#define m_check_str_nrange(_ptr1, _ptr2, _ptr3, _size)			\
	___m_check(M_STR_NRANGE, (__func__), (__LINE__), (char *)(_ptr1), (char *)(_ptr2), (char *)(_ptr3), (size_t)(_size))
/** @} */
#endif
