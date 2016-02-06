/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 * @file mamma.h
 */

#ifndef __M_H__
#define __M_H__

#include <setjmp.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))


/**
 * List of predefined assertion type
 */
enum m_asserts {
	M_CUSTOM = 0,
	M_TRUE,
	M_FALSE,
	M_INT_EQ,
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


enum m_state_machine {
	M_STATE_TEST_SET_UP = 0, /**> Entry point state */
	M_STATE_TEST_RUN,
	M_STATE_TEST_TEAR_DOWN,
	M_STATE_TEST_ERROR,
	M_STATE_TEST_SKIP,
	M_STATE_TEST_EXIT,
	_M_STATE_MAX,
};


#define M_FLAG_CONT_ON_ERROR (0)
#define M_FLAG_STOP_ON_ERROR (1 << 0)

/**
 * Data structure representing a functionality test
 */
struct m_test {
	enum m_state_machine state_cur; /**< current test state */
	enum m_state_machine state_prv; /**< current test state */
	struct m_suite *suite; /**< Suite test where this test belong to */
	void *private; /**< private data that can be used by set_up() and
			  tear_down() functions in order to pass data to
			  the test function */
	void (*set_up)(struct m_test *test); /**< run a setting up function that
						prepare the environment to run
						the test */
	void (*test)(struct m_test *test); /**< test function */
	void (*tear_down)(struct m_test *test); /**< it should reverse the
						   operations done by the
						   set_up() function */
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
	unsigned long flags; /**< suite options */
	void *private; /**< private data that can be used by set_up() and
			  tear_down() functions in order to pass data to
			  the tests */
	struct m_test *tests; /**< list of tests */
	const unsigned int test_count; /**< number of valid tests */
	void (*set_up)(struct m_suite *m_suite); /**< run a setting up function
						    that prepare the environment
						    to run the tests */
	void (*tear_down)(struct m_suite *m_suite); /**< it should reverse the
						       operations done by the
						       set_up() function */
	char *(*strerror)(int errnum); /**< function to use to print errno
					  error messages */
	unsigned int total_count; /**< total number of executed suite's tests */
	unsigned int success_count; /**< number of successful suite's tests */
	unsigned int fail_count; /**< number of failed suite's tests */
	unsigned int skip_count;  /**< number of skipped suite's tests */
	struct m_suite *subgroup;
	unsigned int n_subgroup;
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
extern void m_skip_test(unsigned int cond,
			 const char *func, const unsigned int line);

#define m_skip(_cond) m_skip_test((_cond), __func__, __LINE__)

extern void m_check(enum m_asserts type, unsigned long flags,
		    const char *func, const unsigned int line,
		    ...);


/**
 * @defgroup m_assert_custom Build Custum Assertions
 */

/** @} */

/**
 * @defgroup m_assert_boolean Boolean Assertions
 */
#define m_assert_custom(_cond, _errno, _fmt, ...)		\
        m_check(M_CUSTOM, M_FLAG_STOP_ON_ERROR,			\
		(__func__), (__LINE__), !!(_cond), (_errno),	\
		(_fmt), __VA_ARGS__)
#define m_check_custom(_cond, _errno, _fmt, ...)		\
        m_check(M_CUSTOM, M_FLAG_CONT_ON_ERROR,			\
		(__func__), (__LINE__), !!(_cond), (_errno),	\
		(_fmt), __VA_ARGS__)
/** @} */

/**
 * @defgroup m_assert_boolean Boolean Assertions
 */


#define m_assert_true(_cond)				\
	m_check(M_TRUE, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (_cond))
#define m_assert_false(_cond)				\
	m_check(M_FALSE, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (_cond))

#define m_check_true(_cond)				\
	m_check(M_TRUE, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (_cond))
#define m_check_false(_cond)				\
	m_check(M_FALSE, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (_cond))


#define m_assert_int_eq(_exp, _val)			\
	m_check(M_INT_EQ, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (_exp), (_val))
#define m_assert_int_neq(_exp, _val)			\
	m_check(M_INT_NEQ, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__), (_exp), (_val))
#define m_assert_int_gt(_exp, _val)			\
	m_check(M_INT_GT, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (_exp), (_val))
#define m_assert_int_ge(_exp, _val)			\
	m_check(M_INT_GE, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (_exp), (_val))
#define m_assert_int_lt(_exp, _val)			\
	m_check(M_INT_LT, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (_exp), (_val))
#define m_assert_int_le(_exp, _val)			\
	m_check(M_INT_LE, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (_exp), (_val))
#define m_assert_int_range(_min, _max, _val)			\
	m_check(M_INT_RANGE, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (_min), (_max), (_val))
#define m_assert_int_nrange(_min, _max, _val)			\
	m_check(M_INT_NRANGE, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (_min), (_max), (_val))

#define m_check_int_eq(_exp, _val)			\
	m_check(M_INT_EQ, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (_exp), (_val))
#define m_check_int_neq(_exp, _val)			\
	m_check(M_INT_NEQ, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__), (_exp), (_val))
#define m_check_int_gt(_exp, _val)			\
	m_check(M_INT_GT, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (_exp), (_val))
#define m_check_int_ge(_exp, _val)			\
	m_check(M_INT_GE, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (_exp), (_val))
#define m_check_int_lt(_exp, _val)			\
	m_check(M_INT_LT, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (_exp), (_val))
#define m_check_int_le(_exp, _val)			\
	m_check(M_INT_LE, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (_exp), (_val))
#define m_check_int_range(_min, _max, _val)			\
	m_check(M_INT_RANGE, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (_min), (_max), (_val))
#define m_check_int_nrange(_min, _max, _val)			\
	m_check(M_INT_NRANGE, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (_min), (_max), (_val))


#define m_assert_dbl_eq(_exp, _val)		\
	m_check(M_DBL_EQ, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
#define m_assert_dbl_neq(_exp, _val)			\
	m_check(M_DBL_NEQ, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__),			\
		(double)(_exp), (double)(_val))
#define m_assert_dbl_gt(_exp, _val)		\
	m_check(M_DBL_GT, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
#define m_assert_dbl_ge(_exp, _val)		\
	m_check(M_DBL_GE, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
#define m_assert_dbl_lt(_exp, _val)		\
	m_check(M_DBL_LT, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
#define m_assert_dbl_le(_exp, _val)		\
	m_check(M_DBL_LE, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
#define m_assert_dbl_range(_min, _max, _val)			\
	m_check(M_DBL_RANGE, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__),				\
		(double)(_min), (double)(_max), (double)(_val))
#define m_assert_dbl_nrange(_min, _max, _val)			\
	m_check(M_DBL_NRANGE, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__),				\
		(double)(_min), (double)(_max), (double)(_val))

#define m_check_dbl_eq(_exp, _val)		\
	m_check(M_DBL_EQ, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
#define m_check_dbl_neq(_exp, _val)			\
	m_check(M_DBL_NEQ, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),			\
		(double)(_exp), (double)(_val))
#define m_check_dbl_gt(_exp, _val)		\
	m_check(M_DBL_GT, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
#define m_check_dbl_ge(_exp, _val)		\
	m_check(M_DBL_GE, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
#define m_check_dbl_lt(_exp, _val)		\
	m_check(M_DBL_LT, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
#define m_check_dbl_le(_exp, _val)		\
	m_check(M_DBL_LE, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
#define m_check_dbl_range(_min, _max, _val)			\
	m_check(M_DBL_RANGE, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__),				\
		(double)(_min), (double)(_max), (double)(_val))
#define m_check_dbl_nrange(_min, _max, _val)			\
	m_check(M_DBL_NRANGE, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__),				\
		(double)(_min), (double)(_max), (double)(_val))



#define m_assert_mem_not_null(_ptr)			\
	m_check(M_PTR_NOT_NULL, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__),			\
		(void *)(_ptr))
#define m_assert_mem_null(_ptr)				\
	m_check(M_PTR_NULL, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__),			\
		(void *)(_ptr))
#define m_assert_mem_eq(_ptr1, _ptr2, _size)				\
	m_check(M_MEM_EQ, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_assert_mem_neq(_ptr1, _ptr2, _size)				\
	m_check(M_MEM_NEQ, M_FLAG_STOP_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_assert_mem_gt(_ptr1, _ptr2, _size)				\
	m_check(M_MEM_GT, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_assert_mem_ge(_ptr1, _ptr2, _size)				\
	m_check(M_MEM_GE, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_assert_mem_lt(_ptr1, _ptr2, _size)				\
	m_check(M_MEM_LT, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_assert_mem_le(_ptr1, _ptr2, _size)				\
	m_check(M_MEM_LE, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_assert_mem_range(_ptr1, _ptr2, _ptr3, _size)			\
	m_check(M_MEM_RANGE, M_FLAG_STOP_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(void *)(_ptr1), (void *)(_ptr2), (void *)(_ptr3),	\
		(size_t)(_size))
#define m_assert_mem_nrange(_ptr1, _ptr2, _ptr3, _size)			\
	m_check(M_MEM_NRANGE, M_FLAG_STOP_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(void *)(_ptr1), (void *)(_ptr2), (void *)(_ptr3),	\
		(size_t)(_size))

#define m_check_mem_not_null(_ptr)			\
	m_check(M_PTR_NOT_NULL, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),			\
		(void *)(_ptr))
#define m_check_mem_null(_ptr)				\
	m_check(M_PTR_NULL, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),			\
		(void *)(_ptr))
#define m_check_mem_eq(_ptr1, _ptr2, _size)				\
	m_check(M_MEM_EQ, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_check_mem_neq(_ptr1, _ptr2, _size)				\
	m_check(M_MEM_NEQ, M_FLAG_CONT_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_check_mem_gt(_ptr1, _ptr2, _size)				\
	m_check(M_MEM_GT, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_check_mem_ge(_ptr1, _ptr2, _size)				\
	m_check(M_MEM_GE, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_check_mem_lt(_ptr1, _ptr2, _size)				\
	m_check(M_MEM_LT, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_check_mem_le(_ptr1, _ptr2, _size)				\
	m_check(M_MEM_LE, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_ptr1), (void *)(_ptr2), (size_t)(_size))
#define m_check_mem_range(_ptr1, _ptr2, _ptr3, _size)			\
	m_check(M_MEM_RANGE, M_FLAG_CONT_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(void *)(_ptr1), (void *)(_ptr2), (void *)(_ptr3),	\
		(size_t)(_size))
#define m_check_mem_nrange(_ptr1, _ptr2, _ptr3, _size)			\
	m_check(M_MEM_NRANGE, M_FLAG_CONT_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(void *)(_ptr1), (void *)(_ptr2), (void *)(_ptr3),	\
		(size_t)(_size))



#define m_assert_str_not_null(_ptr) m_assert_mem_not_null((_ptr))
#define m_assert_str_null(_ptr) m_assert_mem_null((_ptr))
#define m_assert_str_eq(_ptr1, _ptr2, _size)				\
	m_check(M_STR_EQ, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_assert_str_neq(_ptr1, _ptr2, _size)				\
	m_check(M_STR_NEQ, M_FLAG_STOP_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_assert_str_gt(_ptr1, _ptr2, _size)				\
	m_check(M_STR_GT, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_assert_str_ge(_ptr1, _ptr2, _size)				\
	m_check(M_STR_GE, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_assert_str_lt(_ptr1, _ptr2, _size)				\
	m_check(M_STR_LT, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_assert_str_le(_ptr1, _ptr2, _size)				\
	m_check(M_STR_LE, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_assert_str_range(_ptr1, _ptr2, _ptr3, _size)			\
	m_check(M_STR_RANGE, M_FLAG_STOP_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(char *)(_ptr1), (char *)(_ptr2), (char *)(_ptr3),	\
		(size_t)(_size))
#define m_assert_str_nrange(_ptr1, _ptr2, _ptr3, _size)			\
	m_check(M_STR_NRANGE, M_FLAG_STOP_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(char *)(_ptr1), (char *)(_ptr2), (char *)(_ptr3),	\
		(size_t)(_size))

#define m_check_str_not_null(_ptr)			\
	m_check(M_PTR_NOT_NULL, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),			\
		(char *)(_ptr))
#define m_check_str_null(_ptr)				\
	m_check(M_PTR_NULL, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),			\
		(char *)(_ptr))
#define m_check_str_eq(_ptr1, _ptr2, _size)				\
	m_check(M_STR_EQ, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_check_str_neq(_ptr1, _ptr2, _size)				\
	m_check(M_STR_NEQ, M_FLAG_CONT_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_check_str_gt(_ptr1, _ptr2, _size)				\
	m_check(M_STR_GT, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_check_str_ge(_ptr1, _ptr2, _size)				\
	m_check(M_STR_GE, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_check_str_lt(_ptr1, _ptr2, _size)				\
	m_check(M_STR_LT, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_check_str_le(_ptr1, _ptr2, _size)				\
	m_check(M_STR_LE, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_ptr1), (char *)(_ptr2), (size_t)(_size))
#define m_check_str_range(_ptr1, _ptr2, _ptr3, _size)			\
	m_check(M_STR_RANGE, M_FLAG_CONT_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(char *)(_ptr1), (char *)(_ptr2), (char *)(_ptr3),	\
		(size_t)(_size))
#define m_check_str_nrange(_ptr1, _ptr2, _ptr3, _size)			\
	m_check(M_STR_NRANGE, M_FLAG_CONT_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(char *)(_ptr1), (char *)(_ptr2), (char *)(_ptr3),	\
		(size_t)(_size))

#endif
