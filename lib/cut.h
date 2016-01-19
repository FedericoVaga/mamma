/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */

#ifndef __CUT_H__
#define __CUT_H__

#define CUT_NAME_MAX_LEN 32
#define CUT_TEST_MAX 128

struct cut_test {
	char name[CUT_NAME_MAX_LEN];
	void *private;
	void (*set_up)(struct cut_test *test);
	void (*test)(struct cut_test *test);
	void (*tear_down)(struct cut_test *test);
};


/**
 * Test suite container. It is a collection of tests
 */
struct cut_suite {
	char name[CUT_NAME_MAX_LEN]; /**< suite name */
	struct cut_test *tests[CUT_TEST_MAX]; /**< list of tests */
	unsigned int test_count; /**< number of valid tests */
};


struct cut_suite *cut_suite_create(char *name);
void cut_suite_destroy(struct cut_suite *cut_suite);

#endif
