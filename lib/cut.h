/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */

#ifndef __CUT_H__
#define __CUT_H__

#include <setjmp.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

enum cut_test_state {
	CUT_STATE_STOP = 0,
	CUT_STATE_RUNNING,
	CUT_STATE_ERROR,
	CUT_STATE_SUCCESS,
};

enum cut_test_jump_conditions {
	CUT_NO_JUMP = 0,
	CUT_JUMP_ERROR,
	CUT_JUMP_TEAR_FAIL,
};

struct cut_test {
	const char *name;
	enum cut_test_state state;
	struct cut_suite *suite;
	void *private;
	void (*set_up)(struct cut_test *test);
	void (*test)(struct cut_test *test);
	void (*tear_down)(struct cut_test *test);
	jmp_buf jbuf;
};
#define cut_test_declare(_name, _up, _down, _test, _priv) {	\
		.name = (_name),				\
			.private = (_priv),			\
			.set_up = (_up),			\
			.test = (_test),			\
			.tear_down = (_down),			\
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
	unsigned int success_count;
	unsigned int fail_count;
};
#define cut_suite_declare(_name, _test, _test_count, _up, _down, _priv) { \
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

extern void _cut_assert(struct cut_test *cut_test, unsigned int condition,
			char *msg, const char *func, unsigned int line);
#define cut_assert(_ct, _cond, _msg, _f, _l) _cut_assert((_ct), (_cond), \
							 (_msg), __func__, \
							 __LINE__)
extern void _cut_assert_int_eq(struct cut_test *cut_test, int exp, int val,
			       const char *func, unsigned int line);
#define cut_assert_int_eq(_ct, _exp, _val) _cut_assert_int_eq((_ct), (_exp), \
							      (_val), __func__, \
							      __LINE__)

#endif
