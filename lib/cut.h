/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */

#ifndef __CUT_H__
#define __CUT_H__

#define CUT_NAME_MAX_LEN 32
#define CUT_TEST_MAX 128

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

struct cut_test {
	char name[CUT_NAME_MAX_LEN];
	struct cut_suite *suite;
	void *private;
	void (*set_up)(struct cut_test *test);
	void (*test)(struct cut_test *test);
	void (*tear_down)(struct cut_test *test);
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
	char name[CUT_NAME_MAX_LEN]; /**< suite name */
	void *private;
	struct cut_test *tests; /**< list of tests */
	unsigned int test_count; /**< number of valid tests */
	void (*set_up)(struct cut_suite *cut_suite);
	void (*tear_down)(struct cut_suite *cut_suite);
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

extern void cut_suite_init(struct cut_suite *suite);
extern void cut_suite_run(struct cut_suite *cut_suite, unsigned long flags);

#endif
