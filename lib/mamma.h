/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 * @file mamma.h
 */

#ifndef __M_H__
#define __M_H__

#include <errno.h>
#include <setjmp.h>

/**
 * It computes the given array size
 */
#define M_ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

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
	M_ERR_EQ,
	M_ERR_NEQ,
	__M_MAX_STANDARD_ASSERTION,
};


/**
 * List of all possible state machine states
 */
enum m_state_machine {
	M_STATE_SUITE_SET_UP = 0,
	M_STATE_TEST_SET_UP, /**> Test entry point state */
	M_STATE_TEST_RUN,
	M_STATE_TEST_TEAR_DOWN,
	M_STATE_TEST_ERROR,
	M_STATE_TEST_SKIP,
	M_STATE_TEST_EXIT, /**> Test exit point state */
	M_STATE_SUITE_TEAR_DOWN,
	M_STATE_SUITE_EXIT,
	_M_STATE_MAX,
};

enum m_state_machine_test_exit_cause {
	M_STATE_EXIT_NORUN = 0,
	M_STATE_EXIT_SUCCESS,
	M_STATE_EXIT_SKIP,
	M_STATE_EXIT_ERROR,
};

/**
 * It tells the state-machine to continue the execution on error
 */
#define M_FLAG_CONT_ON_ERROR (0)

/**
 * It tells the state-machine to stop the execution on error
 */
#define M_FLAG_STOP_ON_ERROR (1 << 0)


/**
 * Data structure representing a functionality test
 */
struct m_test {
	const char *desc; /**< test description */
	unsigned int index; /**< test index within the test suite */
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
	unsigned int loop; /**< number of test repetitions */
	enum m_state_machine_test_exit_cause exit;
	unsigned int warnings;
};

/**
 * It declare a test in a shorter way
 * @param[in] _up set_up function to assign
 * @param[in] _test test function to assign
 * @param[in] _down tear_down function to assign
 */
#define m_test(_up, _test, _down) {		\
			.desc = NULL,		\
			.set_up = (_up),	\
			.test = (_test),	\
			.tear_down = (_down),	\
			.loop = 1,		\
			}

/**
 * It declare a test in a shorter way
 * @param[in] _up set_up function to assign
 * @param[in] _test test function to assign
 * @param[in] _down tear_down function to assign
 * @param[in] _desc test description
 */
#define m_test_desc(_up, _test, _down, _desc) {	\
			.desc = (_desc),	\
			.set_up = (_up),	\
			.test = (_test),	\
			.tear_down = (_down),	\
			.loop = 1,		\
			}

/**
 * It declare a test in a shorter way
 * @param[in] _up set_up function to assign
 * @param[in] _test test function to assign
 * @param[in] _down tear_down function to assign
 * @param[in] _loop test repetitions
 */
#define m_test_loop(_up, _test, _down, _loop) { \
			.desc = NULL,           \
			.set_up = (_up),        \
			.test = (_test),        \
			.tear_down = (_down),   \
			.loop = (_loop),        \
			}

/**
 * It declare a test in a shorter way
 * @param[in] _up set_up function to assign
 * @param[in] _test test function to assign
 * @param[in] _down tear_down function to assign
 * @param[in] _loop test repetitions
 */
#define m_test_desc_loop(_up, _test, _down, _desc, _loop) {\
			.desc = (_desc),        \
			.set_up = (_up),        \
			.test = (_test),        \
			.tear_down = (_down),   \
			.loop = (_loop),        \
			}

/**
 * Test suite container. It is a collection of tests
 */
struct m_suite {
	const char *name; /**< suite name */
	const char *desc; /**> suite description */
	unsigned long flags; /**< suite options */
	void *private; /**< private data that can be used by set_up() and
			  tear_down() functions in order to pass data to
			  the tests */
	struct m_test *tests; /**< list of tests */
	unsigned int test_count; /**< number of valid tests */
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
};

/**
 * It declare a test-suite in a shorter way
 * @param[in] _name suite name
 * @param[in] _flags customize test-suite execution
 * @param[in] _tests list of tests to assign to the test-suite
 * @param[in] _up set_up function to assign
 * @param[in] _down tear_down function to assign
 */
#define m_suite(_name, _flags, _tests, _up, _down) {		\
			.name = (_name),			\
			.flags = (_flags),			\
			.tests = (_tests),			\
			.test_count = M_ARRAY_SIZE((_tests)),	\
			.set_up = (_up),			\
			.tear_down = (_down),			\
			.strerror = NULL,			\
			}

/**
 * It prints out more information
 */
#define M_VERBOSE (1 << 0)

/**
 * It prints also ERRNO messages associated to the check/assert
 */
#define M_ERRNO_CHECK (1 << 1)
#define M_ERRNO M_ERRNO_CHECK

/**
 * It prints also ERRNO messages  from last function call
 */
#define M_ERRNO_FUNC (1 << 2)

extern void m_test_run(struct m_test *m_test);
extern void m_suite_run(struct m_suite *m_suite);
extern void m_skip_test(unsigned int cond,
			 const char *func, const unsigned int line);

/**
 * It skip the test if the given condition is true
 * @param[in] _cond condition to verify
 */
#define m_skip(_cond) m_skip_test((_cond), __func__, __LINE__)

extern void m_check(enum m_asserts type, unsigned long flags,
		    const char *func, const unsigned int line,
		    ...);


/**
 * @addtogroup m_assert_custom Build Custum Assertions
 */

/** @} */

#define m_assert_custom(_cond, _errno, _fmt, ...)		\
        m_check(M_CUSTOM, M_FLAG_STOP_ON_ERROR,			\
		(__func__), (__LINE__), !!(_cond), (_errno),	\
		(_fmt), __VA_ARGS__)
#define m_check_custom(_cond, _errno, _fmt, ...)		\
        m_check(M_CUSTOM, M_FLAG_CONT_ON_ERROR,			\
		(__func__), (__LINE__), !!(_cond), (_errno),	\
		(_fmt), __VA_ARGS__)


/**
 * @addtogroup m_assert_boolean Boolean Assertions and Checks
 * @{
 */

/**
 * If the given condition is not true it raise an error and it stops
 * test execution
 * @param[in] _cond condition to evaluate
 */
#define m_assert_true(_cond)				\
	m_check(M_TRUE, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (_cond))
/**
 * If the given condition is not false it raise an error and it stops
 * test execution
 * @param[in] _cond condition to evaluate
 */
#define m_assert_false(_cond)				\
	m_check(M_FALSE, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (_cond))
/**
 * If the given condition is not true it raise an error
 * @param[in] _cond condition to evaluate
 */
#define m_check_true(_cond)				\
	m_check(M_TRUE, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (_cond))
/**
 * If the given condition is not false it raise an error
 * @param[in] _cond condition to evaluate
 */
#define m_check_false(_cond)				\
	m_check(M_FALSE, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (_cond))
/** @} */


/**
 * @addtogroup m_assert_int Integer Assertions and Checks
 * @{
 */

/**
 * If the given values are not equal it raise an error and it stops
 * test execution
 *
 * _exp == _val  OK
 *
 * _exp != _val  Error
 *
 * @param[in] _exp expected value
 * @param[in] _val value to compare with
 */
#define m_assert_int_eq(_exp, _val)			\
	m_check(M_INT_EQ, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (long)(_exp), (long)(_val))
/**
 * If the given values are equal it raise an error and it stops
 * test execution
 *
 * _exp != _val  OK
 *
 * _exp == _val  Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_assert_int_neq(_exp, _val)			\
	m_check(M_INT_NEQ, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__), (long)(_exp), (long)(_val))
/**
 * If the expected value is not greater than the given one it raise an error
 * and it stops test execution
 *
 * _exp > _val  OK
 *
 * _exp <= _val Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_assert_int_gt(_exp, _val)			\
	m_check(M_INT_GT, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (long)(_exp), (long)(_val))
/**
 * If the expected value is not greater or equal than the given one it raise
 * an error and it stops test execution
 *
 * _exp >= _val  OK
 *
 * _exp < _val Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_assert_int_ge(_exp, _val)			\
	m_check(M_INT_GE, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (long)(_exp), (long)(_val))
/**
 * If the expected value is not less than the given one it raise an error
 * and it stops test execution
 *
 * _exp < _val  OK
 *
 * _exp >= _val Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_assert_int_lt(_exp, _val)			\
	m_check(M_INT_LT, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (long)(_exp), (long)(_val))
/**
 * If the expected value is not less or equal than the given one it raise
 * an error and it stops test execution
 *
 * _exp <= _val  OK
 *
 * _exp > _val Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_assert_int_le(_exp, _val)			\
	m_check(M_INT_LE, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (long)(_exp), (long)(_val))
/**
 * If the given value is not within the range it raise an error
 * and it stops test execution
 *
 * _min <= _val <= _max  OK
 *
 * _min > _val || _max < _val Error
 *
 * @param[in] _min minimum expected value
 * @param[in] _max maximum expected value
 * @param[in] _val value to compare with
 */
#define m_assert_int_range(_min, _max, _val)			\
	m_check(M_INT_RANGE, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (long)(_min), (long)(_max), (long)(_val))
/**
 * If the given value is within the range it raise an error
 * and it stops test execution
 *
 * _min > _val || _max < _val OK
 *
 * _min <= _val <= _max  Error
 *
 * @param[in] _min minimum expected value
 * @param[in] _max maximum expected value
 * @param[in] _val value to compare with
 */
#define m_assert_int_nrange(_min, _max, _val)			\
	m_check(M_INT_NRANGE, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (long)(_min), (long)(_max), (long)(_val))

/**
 * If the given values are not equal it raise an error
 *
 * _exp == _val  OK
 *
 * _exp != _val  Error
 *
 * @param[in] _exp expected value
 * @param[in] _val value to compare with
 */
#define m_check_int_eq(_exp, _val)			\
	m_check(M_INT_EQ, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (long)(_exp), (long)(_val))
/**
 * If the given values are equal it raise an error
 *
 * _exp != _val  OK
 *
 * _exp == _val  Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_check_int_neq(_exp, _val)			\
	m_check(M_INT_NEQ, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__), (long)(_exp), (long)(_val))
/**
 * If the expected value is not greater than the given one it raise an error
 *
 * _exp > _val  OK
 *
 * _exp <= _val Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_check_int_gt(_exp, _val)			\
	m_check(M_INT_GT, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (long)(_exp), (long)(_val))
/**
 * If the expected value is not greater or equal than the given one it raise
 * an error
 *
 * _exp >= _val  OK
 *
 * _exp < _val Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_check_int_ge(_exp, _val)			\
	m_check(M_INT_GE, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (long)(_exp), (long)(_val))
/**
 * If the expected value is not less than the given one it raise an error
 *
 * _exp < _val  OK
 *
 * _exp >= _val Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_check_int_lt(_exp, _val)			\
	m_check(M_INT_LT, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (long)(_exp), (long)(_val))
/**
 * If the expected value is not less or equal than the given one it raise
 * an error
 *
 * _exp <= _val  OK
 *
 * _exp > _val Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_check_int_le(_exp, _val)			\
	m_check(M_INT_LE, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (long)(_exp), (long)(_val))
/**
 * If the given value is not within the range it raise an error
 *
 * _min <= _val <= _max  OK
 *
 * _min > _val || _max < _val Error
 *
 * @param[in] _min minimum expected value
 * @param[in] _max maximum expected value
 * @param[in] _val value to compare with
 */
#define m_check_int_range(_min, _max, _val)			\
	m_check(M_INT_RANGE, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (long)(_min), (long)(_max), (long)(_val))
/**
 * If the given value is within the range it raise an error
 *
 * _min > _val || _max < _val OK
 *
 * _min <= _val <= _max  Error
 *
 * @param[in] _min minimum expected value
 * @param[in] _max maximum expected value
 * @param[in] _val value to compare with
 */
#define m_check_int_nrange(_min, _max, _val)			\
	m_check(M_INT_NRANGE, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (long)(_min), (long)(_max), (long)(_val))
/** @} */


/**
 * @addtogroup m_assert_dbl Dobule Assertions and Checks
 * @{
 */
/**
 * If the given values are not equal it raise an error and it stops
 * test execution
 *
 * _exp == _val  OK
 *
 * _exp != _val  Error
 *
 * @param[in] _exp expected value
 * @param[in] _val value to compare with
 */
#define m_assert_dbl_eq(_exp, _val)		\
	m_check(M_DBL_EQ, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
/**
 * If the given values are equal it raise an error and it stops
 * test execution
 *
 * _exp != _val  OK
 *
 * _exp == _val  Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_assert_dbl_neq(_exp, _val)			\
	m_check(M_DBL_NEQ, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__),			\
		(double)(_exp), (double)(_val))
/**
 * If the expected value is not greater than the given one it raise an error
 * and it stops test execution
 *
 * _exp > _val  OK
 *
 * _exp <= _val Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_assert_dbl_gt(_exp, _val)		\
	m_check(M_DBL_GT, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
/**
 * If the expected value is not greater or equal than the given one it raise
 * an error and it stops test execution
 *
 * _exp >= _val  OK
 *
 * _exp < _val Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_assert_dbl_ge(_exp, _val)		\
	m_check(M_DBL_GE, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
/**
 * If the expected value is not less than the given one it raise an error
 * and it stops test execution
 *
 * _exp < _val  OK
 *
 * _exp >= _val Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_assert_dbl_lt(_exp, _val)		\
	m_check(M_DBL_LT, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
/**
 * If the expected value is not less or equal than the given one it raise
 * an error and it stops test execution
 *
 * _exp <= _val  OK
 *
 * _exp > _val Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_assert_dbl_le(_exp, _val)		\
	m_check(M_DBL_LE, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
/**
 * If the given value is not within the range it raise an error
 * and it stops test execution
 *
 * _min <= _val <= _max  OK
 *
 * _min > _val || _max < _val Error
 *
 * @param[in] _min minimum expected value
 * @param[in] _max maximum expected value
 * @param[in] _val value to compare with
 */
#define m_assert_dbl_range(_min, _max, _val)			\
	m_check(M_DBL_RANGE, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__),				\
		(double)(_min), (double)(_max), (double)(_val))
/**
 * If the given value is within the range it raise an error
 * and it stops test execution
 *
 * _min > _val || _max < _val OK
 *
 * _min <= _val <= _max  Error
 *
 * @param[in] _min minimum expected value
 * @param[in] _max maximum expected value
 * @param[in] _val value to compare with
 */
#define m_assert_dbl_nrange(_min, _max, _val)			\
	m_check(M_DBL_NRANGE, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__),				\
		(double)(_min), (double)(_max), (double)(_val))
/**
 * If the given values are not equal it raise an error
 *
 * _exp == _val  OK
 *
 * _exp != _val  Error
 *
 * @param[in] _exp expected value
 * @param[in] _val value to compare with
 */
#define m_check_dbl_eq(_exp, _val)		\
	m_check(M_DBL_EQ, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
/**
 * If the given values are equal it raise an error
 *
 * _exp != _val  OK
 *
 * _exp == _val  Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_check_dbl_neq(_exp, _val)			\
	m_check(M_DBL_NEQ, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),			\
		(double)(_exp), (double)(_val))
/**
 * If the expected value is not greater than the given one it raise an error
 *
 * _exp > _val  OK
 *
 * _exp <= _val Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_check_dbl_gt(_exp, _val)		\
	m_check(M_DBL_GT, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
/**
 * If the expected value is not greater or equal than the given one it raise
 * an error
 *
 * _exp >= _val  OK
 *
 * _exp < _val Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_check_dbl_ge(_exp, _val)		\
	m_check(M_DBL_GE, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
/**
 * If the expected value is not less than the given one it raise an error
 *
 * _exp < _val  OK
 *
 * _exp >= _val Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_check_dbl_lt(_exp, _val)		\
	m_check(M_DBL_LT, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
/**
 * If the expected value is not less or equal than the given one it raise
 * an error
 *
 * _exp <= _val  OK
 *
 * _exp > _val Error
 *
 * @param[in] _exp not expected value
 * @param[in] _val value to compare with
 */
#define m_check_dbl_le(_exp, _val)		\
	m_check(M_DBL_LE, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
/**
 * If the given value is not within the range it raise an error
 *
 * _min <= _val <= _max  OK
 *
 * _min > _val || _max < _val Error
 *
 * @param[in] _min minimum expected value
 * @param[in] _max maximum expected value
 * @param[in] _val value to compare with
 */
#define m_check_dbl_range(_min, _max, _val)			\
	m_check(M_DBL_RANGE, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__),				\
		(double)(_min), (double)(_max), (double)(_val))
/**
 * If the given value is within the range it raise an error
 *
 * _min > _val || _max < _val OK
 *
 * _min <= _val <= _max  Error
 *
 * @param[in] _min minimum expected value
 * @param[in] _max maximum expected value
 * @param[in] _val value to compare with
 */
#define m_check_dbl_nrange(_min, _max, _val)			\
	m_check(M_DBL_NRANGE, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__),				\
		(double)(_min), (double)(_max), (double)(_val))
/** @} */

/**
 * @addtogroup m_assert_mem Memory Assertions and Checks
 * @{
 */

/**
 * If the given pointer is NULL it raises an error and it stops
 * test execution
 *
 * _ptr != NULL OK
 *
 * _ptr == NULL Error
 *
 * @param[in] _ptr pointer to evaluate
 */
#define m_assert_mem_not_null(_ptr)			\
	m_check(M_PTR_NOT_NULL, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__),			\
		(void *)(_ptr))
/**
 * If the given pointer is not NULL it raises an error and it stops
 * test execution
 *
 * _ptr == NULL OK
 *
 * _ptr != NULL Error
 *
 * @param[in] _ptr pointer to evaluate
 */
#define m_assert_mem_null(_ptr)				\
	m_check(M_PTR_NULL, M_FLAG_STOP_ON_ERROR,	\
		(__func__), (__LINE__),			\
		(void *)(_ptr))
/**
 * If the given memory areas are not equal it raise an error and it stops
 * test execution
 *
 * _exp == _val  OK
 *
 * _exp != _val  Error
 *
 * @param[in] _exp memory area with the expected value
 * @param[in] _val memory area to compare with
 * @param[in] _size memory size to evaluate
 */
#define m_assert_mem_eq(_exp, _val, _size)				\
	m_check(M_MEM_EQ, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_exp), (void *)(_val), (size_t)(_size))
/**
 * If the given memory areas are equal it raise an error and it stops
 * test execution
 *
 * _exp != _val  OK
 *
 * _exp == _val  Error
 *
 * @param[in] _exp memory area with the expected value
 * @param[in] _val memory area to compare with
 * @param[in] _size memory size to evaluate
 */
#define m_assert_mem_neq(_exp, _val, _size)				\
	m_check(M_MEM_NEQ, M_FLAG_STOP_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(void *)(_exp), (void *)(_val), (size_t)(_size))
/**
 * If the expected memory area content is not greater than the given one
 * it raise an error and it stops test execution
 *
 * _exp > _val  OK
 *
 * _exp <= _val Error
 *
 * @param[in] _exp memory area with the expected value
 * @param[in] _val memory area to compare with
 * @param[in] _size memory size to evaluate
 */
#define m_assert_mem_gt(_exp, _val, _size)				\
	m_check(M_MEM_GT, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_exp), (void *)(_val), (size_t)(_size))
/**
 * If the expected memory area content is not greater or equal than the given
 * one it raise an error and it stops test execution
 *
 * _exp >= _val  OK
 *
 * _exp < _val Error
 *
 * @param[in] _exp memory area with the expected value
 * @param[in] _val memory area to compare with
 * @param[in] _size memory size to evaluate
 */
#define m_assert_mem_ge(_exp, _val, _size)				\
	m_check(M_MEM_GE, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_exp), (void *)(_val), (size_t)(_size))
/**
 * If the expected memory area content is not less than the given
 * one it raise an error and it stops test execution
 *
 * _exp < _val  OK
 *
 * _exp >= _val Error
 *
 * @param[in] _exp memory area with the expected value
 * @param[in] _val memory area to compare with
 * @param[in] _size memory size to evaluate
 */
#define m_assert_mem_lt(_exp, _val, _size)				\
	m_check(M_MEM_LT, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_exp), (void *)(_val), (size_t)(_size))
/**
 * If the expected memory area content is not less or equal than the given
 * one it raise an error and it stops test execution
 *
 * _exp <= _val  OK
 *
 * _exp > _val Error
 *
 * @param[in] _exp memory area with the expected value
 * @param[in] _val memory area to compare with
 * @param[in] _size memory size to evaluate
 */
#define m_assert_mem_le(_exp, _val, _size)				\
	m_check(M_MEM_LE, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_exp), (void *)(_val), (size_t)(_size))
/**
 * If the given value is not within the range it raise an error and it stops
 * test execution
 *
 * _min <= _val <= _max  OK
 *
 * _min > _val || _max < _val Error
 *
 * @param[in] _min memory area with the expected minimum value
 * @param[in] _max memory area with the expected maximum value
 * @param[in] _val memory area to compare with
 * @param[in] _size memory size to evaluate
 */
#define m_assert_mem_range(_min, _max, _val, _size)			\
	m_check(M_MEM_RANGE, M_FLAG_STOP_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(void *)(_min), (void *)(_max), (void *)(_val),		\
		(size_t)(_size))
/**
 * If the given value is within the range it raise an error and it stops
 * test execution
 *
 * _min > _val || _max < _val OK
 *
 * _min <= _val <= _max  Error
 *
 * @param[in] _min memory area with the expected minimum value
 * @param[in] _max memory area with the expected maximum value
 * @param[in] _val memory area to compare with
 * @param[in] _size memory size to evaluate
 */
#define m_assert_mem_nrange(_min, _max, _val, _size)			\
	m_check(M_MEM_NRANGE, M_FLAG_STOP_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(void *)(_min), (void *)(_max), (void *)(_val),		\
		(size_t)(_size))
/**
 * If the given pointer is NULL it raises an error
 *
 * _ptr != NULL OK
 *
 * _ptr == NULL Error
 *
 * @param[in] _ptr pointer to evaluate
 */
#define m_check_mem_not_null(_ptr)			\
	m_check(M_PTR_NOT_NULL, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),			\
		(void *)(_ptr))
/**
 * If the given pointer is not NULL it raises an error
 *
 * _ptr == NULL OK
 *
 * _ptr != NULL Error
 *
 * @param[in] _ptr pointer to evaluate
 */
#define m_check_mem_null(_ptr)				\
	m_check(M_PTR_NULL, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),			\
		(void *)(_ptr))
/**
 * If the given memory areas are not equal it raise an error
 *
 * _exp == _val  OK
 *
 * _exp != _val  Error
 *
 * @param[in] _exp memory area with the expected value
 * @param[in] _val memory area to compare with
 * @param[in] _size memory size to evaluate
 */
#define m_check_mem_eq(_exp, _val, _size)				\
	m_check(M_MEM_EQ, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_exp), (void *)(_val), (size_t)(_size))
/**
 * If the given memory areas are equal it raise an error
 *
 * _exp != _val  OK
 *
 * _exp == _val  Error
 *
 * @param[in] _exp memory area with the expected value
 * @param[in] _val memory area to compare with
 * @param[in] _size memory size to evaluate
 */
#define m_check_mem_neq(_exp, _val, _size)				\
	m_check(M_MEM_NEQ, M_FLAG_CONT_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(void *)(_exp), (void *)(_val), (size_t)(_size))
/**
 * If the expected memory area content is not greater than the given one
 * it raise an error
 *
 * _exp > _val  OK
 *
 * _exp <= _val Error
 *
 * @param[in] _exp memory area with the expected value
 * @param[in] _val memory area to compare with
 * @param[in] _size memory size to evaluate
 */
#define m_check_mem_gt(_exp, _val, _size)				\
	m_check(M_MEM_GT, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_exp), (void *)(_val), (size_t)(_size))
/**
 * If the expected memory area content is not greater or equal than the given
 * one it raise an error
 *
 * _exp >= _val  OK
 *
 * _exp < _val Error
 *
 * @param[in] _exp memory area with the expected value
 * @param[in] _val memory area to compare with
 * @param[in] _size memory size to evaluate
 */
#define m_check_mem_ge(_exp, _val, _size)				\
	m_check(M_MEM_GE, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_exp), (void *)(_val), (size_t)(_size))
/**
 * If the expected memory area content is not less than the given
 * one it raise an error
 *
 * _exp < _val  OK
 *
 * _exp >= _val Error
 *
 * @param[in] _exp memory area with the expected value
 * @param[in] _val memory area to compare with
 * @param[in] _size memory size to evaluate
 */
#define m_check_mem_lt(_exp, _val, _size)				\
	m_check(M_MEM_LT, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_exp), (void *)(_val), (size_t)(_size))
/**
 * If the expected memory area content is not less or equal than the given
 * one it raise an error
 *
 * _exp <= _val  OK
 *
 * _exp > _val Error
 *
 * @param[in] _exp memory area with the expected value
 * @param[in] _val memory area to compare with
 * @param[in] _size memory size to evaluate
 */
#define m_check_mem_le(_exp, _val, _size)				\
	m_check(M_MEM_LE, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(void *)(_exp), (void *)(_val), (size_t)(_size))
/**
 * If the given value is not within the range it raise an error
 *
 * _min <= _val <= _max  OK
 *
 * _min > _val || _max < _val Error
 *
 * @param[in] _min memory area with the expected minimum value
 * @param[in] _max memory area with the expected maximum value
 * @param[in] _val memory area to compare with
 * @param[in] _size memory size to evaluate
 */
#define m_check_mem_range(_min, _max, _val, _size)			\
	m_check(M_MEM_RANGE, M_FLAG_CONT_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(void *)(_min), (void *)(_max), (void *)(_val),		\
		(size_t)(_size))
/**
 * If the given value is within the range it raise an error
 *
 * _min > _val || _max < _val OK
 *
 * _min <= _val <= _max  Error
 *
 * @param[in] _min memory area with the expected minimum value
 * @param[in] _max memory area with the expected maximum value
 * @param[in] _val memory area to compare with
 * @param[in] _size memory size to evaluate
 */
#define m_check_mem_nrange(_min, _max, _val, _size)			\
	m_check(M_MEM_NRANGE, M_FLAG_CONT_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(void *)(_min), (void *)(_max), (void *)(_val),		\
		(size_t)(_size))
/** @} */


/**
 * @addtogroup m_assert_str String Assertions and Checks
 * @{
 */
/**
 * If the given string is NULL it raises an error and it stops
 * test execution
 *
 * _ptr != NULL OK
 *
 * _ptr == NULL Error
 *
 * @param[in] _ptr pointer to evaluate
 */
#define m_assert_str_not_null(_ptr) m_assert_mem_not_null((_ptr))
/**
 * If the given string is not NULL it raises an error and it stops
 * test execution
 *
 * _ptr == NULL OK
 *
 * _ptr != NULL Error
 *
 * @param[in] _ptr pointer to evaluate
 */
#define m_assert_str_null(_ptr) m_assert_mem_null((_ptr))
/**
 * If the given strings are not equal it raise an error and it stops
 * test execution
 *
 * _exp == _val  OK
 *
 * _exp != _val  Error
 *
 * @param[in] _exp expected strings
 * @param[in] _val stringsto compare with
 * @param[in] _size maximum string length
 */
#define m_assert_str_eq(_exp, _val, _size)				\
	m_check(M_STR_EQ, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_exp), (char *)(_val), (size_t)(_size))
/**
 * If the given strings are equal it raise an error and it stops
 * test execution
 *
 * _exp != _val  OK
 *
 * _exp == _val  Error
 *
 * @param[in] _exp expected strings
 * @param[in] _val stringsto compare with
 * @param[in] _size maximum string length
 */
#define m_assert_str_neq(_exp, _val, _size)				\
	m_check(M_STR_NEQ, M_FLAG_STOP_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(char *)(_exp), (char *)(_val), (size_t)(_size))
/**
 * If the expected string is not greater than the given one
 * it raise an error and it stops test execution
 *
 * _exp > _val  OK
 *
 * _exp <= _val Error
 *
 * @param[in] _exp expected strings
 * @param[in] _val stringsto compare with
 * @param[in] _size maximum string length
 */
#define m_assert_str_gt(_exp, _val, _size)				\
	m_check(M_STR_GT, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_exp), (char *)(_val), (size_t)(_size))
/**
 * If the expected string is not greater or equal than the given
 * one it raise an error and it stops test execution
 *
 * _exp >= _val  OK
 *
 * _exp < _val Error
 *
 * @param[in] _exp expected strings
 * @param[in] _val stringsto compare with
 * @param[in] _size maximum string length
 */
#define m_assert_str_ge(_exp, _val, _size)				\
	m_check(M_STR_GE, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_exp), (char *)(_val), (size_t)(_size))
/**
 * If the expected string is not less than the given
 * one it raise an error and it stops test execution
 *
 * _exp < _val  OK
 *
 * _exp >= _val Error
 *
 * @param[in] _exp expected strings
 * @param[in] _val stringsto compare with
 * @param[in] _size maximum string length
 */
#define m_assert_str_lt(_exp, _val, _size)				\
	m_check(M_STR_LT, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_exp), (char *)(_val), (size_t)(_size))
/**
 * If the expected string is not less or equal than the given
 * one it raise an error and it stops test execution
 *
 * _exp <= _val  OK
 *
 * _exp > _val Error
 *
 * @param[in] _exp expected strings
 * @param[in] _val stringsto compare with
 * @param[in] _size maximum string length
 */
#define m_assert_str_le(_exp, _val, _size)				\
	m_check(M_STR_LE, M_FLAG_STOP_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_exp), (char *)(_val), (size_t)(_size))
/**
 * If the given string is not within the range it raise an error and it stops
 * test execution
 *
 * _min <= _val <= _max  OK
 *
 * _min > _val || _max < _val Error
 *
 * @param[in] _min expected minumum string
 * @param[in] _max expected maximum string
 * @param[in] _val string to compare with
 * @param[in] _size maximum string length
 */
#define m_assert_str_range(_min, _max, _val, _size)			\
	m_check(M_STR_RANGE, M_FLAG_STOP_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(char *)(_min), (char *)(_max), (char *)(_val),		\
		(size_t)(_size))
/**
 * If the given value is within the range it raise an error and it stops
 * test execution
 *
 * _min > _val || _max < _val OK
 *
 * _min <= _val <= _max  Error
 *
 * @param[in] _min expected minumum string
 * @param[in] _max expected maximum string
 * @param[in] _val string to compare with
 * @param[in] _size maximum string length
 */
#define m_assert_str_nrange(_min, _max, _val, _size)			\
	m_check(M_STR_NRANGE, M_FLAG_STOP_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(char *)(_min), (char *)(_max), (char *)(_val),		\
		(size_t)(_size))

/**
 * If the given string is NULL it raises an error
 *
 * _ptr != NULL OK
 *
 * _ptr == NULL Error
 *
 * @param[in] _ptr pointer to evaluate
 */
#define m_check_str_not_null(_ptr)			\
	m_check(M_PTR_NOT_NULL, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),			\
		(char *)(_ptr))
/**
 * If the given string is not NULL it raises an error
 *
 * _ptr == NULL OK
 *
 * _ptr != NULL Error
 *
 * @param[in] _ptr pointer to evaluate
 */
#define m_check_str_null(_ptr)				\
	m_check(M_PTR_NULL, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),			\
		(char *)(_ptr))
/**
 * If the given strings are not equal it raise an error
 *
 * _exp == _val  OK
 *
 * _exp != _val  Error
 *
 * @param[in] _exp expected strings
 * @param[in] _val stringsto compare with
 * @param[in] _size maximum string length
 */
#define m_check_str_eq(_exp, _val, _size)				\
	m_check(M_STR_EQ, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_exp), (char *)(_val), (size_t)(_size))
/**
 * If the given strings are equal it raise an error
 *
 * _exp != _val  OK
 *
 * _exp == _val  Error
 *
 * @param[in] _exp expected strings
 * @param[in] _val stringsto compare with
 * @param[in] _size maximum string length
 */
#define m_check_str_neq(_exp, _val, _size)				\
	m_check(M_STR_NEQ, M_FLAG_CONT_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(char *)(_exp), (char *)(_val), (size_t)(_size))
/**
 * If the expected string is not greater than the given one
 * it raise an error
 *
 * _exp > _val  OK
 *
 * _exp <= _val Error
 *
 * @param[in] _exp expected strings
 * @param[in] _val stringsto compare with
 * @param[in] _size maximum string length
 */
#define m_check_str_gt(_exp, _val, _size)				\
	m_check(M_STR_GT, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_exp), (char *)(_val), (size_t)(_size))
/**
 * If the expected string is not greater or equal than the given
 * one it raise an error
 *
 * _exp >= _val  OK
 *
 * _exp < _val Error
 *
 * @param[in] _exp expected strings
 * @param[in] _val stringsto compare with
 * @param[in] _size maximum string length
 */
#define m_check_str_ge(_exp, _val, _size)				\
	m_check(M_STR_GE, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_exp), (char *)(_val), (size_t)(_size))
/**
 * If the expected string is not less than the given
 * one it raise an error
 *
 * _exp < _val  OK
 *
 * _exp >= _val Error
 *
 * @param[in] _exp expected strings
 * @param[in] _val stringsto compare with
 * @param[in] _size maximum string length
 */
#define m_check_str_lt(_exp, _val, _size)				\
	m_check(M_STR_LT, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_exp), (char *)(_val), (size_t)(_size))
/**
 * If the expected string is not less or equal than the given
 * one it raise an error
 *
 * _exp <= _val  OK
 *
 * _exp > _val Error
 *
 * @param[in] _exp expected strings
 * @param[in] _val stringsto compare with
 * @param[in] _size maximum string length
 */
#define m_check_str_le(_exp, _val, _size)				\
	m_check(M_STR_LE, M_FLAG_CONT_ON_ERROR,				\
		(__func__), (__LINE__),					\
		(char *)(_exp), (char *)(_val), (size_t)(_size))
/**
 * If the given string is not within the range it raise an error
 *
 * _min <= _val <= _max  OK
 *
 * _min > _val || _max < _val Error
 *
 * @param[in] _min expected minumum string
 * @param[in] _max expected maximum string
 * @param[in] _val string to compare with
 * @param[in] _size maximum string length
 */
#define m_check_str_range(_min, _max, _val, _size)			\
	m_check(M_STR_RANGE, M_FLAG_CONT_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(char *)(_min), (char *)(_max), (char *)(_val),		\
		(size_t)(_size))
/**
 * If the given value is within the range it raise an error
 *
 * _min > _val || _max < _val OK
 *
 * _min <= _val <= _max  Error
 *
 * @param[in] _min expected minumum string
 * @param[in] _max expected maximum string
 * @param[in] _val string to compare with
 * @param[in] _size maximum string length
 */
#define m_check_str_nrange(_min, _max, _val, _size)			\
	m_check(M_STR_NRANGE, M_FLAG_CONT_ON_ERROR,			\
		(__func__), (__LINE__),					\
		(char *)(_min), (char *)(_max), (char *)(_val),		\
		(size_t)(_size))
/** @} */


/**
 * @addtogroup m_assert_err Error (errno) Assertions and Checks
 * @{
 */

/**
 * If the given error code is not equal to errno, it raises an error and it
 * stops test execution
 *
 * _exp == errno  OK
 *
 * _exp != errno  Error
 *
 * @param[in] _exp expected error
 */
#define m_assert_errno_eq(_exp)					\
	m_check(M_ERR_EQ, M_FLAG_STOP_ON_ERROR,			\
		(__func__), (__LINE__), (long)(_exp), (long)(errno))

/**
 * If the given error code is equal to errno, it raises an error and it
 * stops test execution
 *
 * _exp != errno  OK
 *
 * _exp == errno  Error
 *
 * @param[in] _exp not expected error
 */
#define m_assert_errno_neq(_exp)					\
	m_check(M_INT_NEQ, M_FLAG_STOP_ON_ERROR,			\
		(__func__), (__LINE__), (long)(_exp), (long)(errno))

/**
 * If the given error code is not equal to errno, it raises an error and it
 * continues test execution
 *
 * _exp == errno  OK
 *
 * _exp != errno  Error
 *
 * @param[in] _exp expected error
 */
#define m_check_errno_eq(_exp)					\
	m_check(M_ERR_EQ, M_FLAG_CONT_ON_ERROR,			\
		(__func__), (__LINE__), (long)(_exp), (long)(errno))

/**
 * If the given error code is equal to errno, it raises an error and it
 * continues test execution
 *
 * _exp != errno  OK
 *
 * _exp == errno  Error
 *
 * @param[in] _exp not expected error
 */
#define m_check_errno_neq(_exp)					\
	m_check(M_ERR_NEQ, M_FLAG_CONT_ON_ERROR,			\
		(__func__), (__LINE__), (long)(_exp), (long)(errno))
/** @} */

#endif
