/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <cut.h>

static void set_up(struct cut_test *cut_test)
{
}
static void tear_down(struct cut_test *cut_test)
{
}

static void test1(struct cut_test *cut_test)
{
	cut_assert_int_eq(4, 4);
	cut_assert_int_eq(4, 5);
}

static void test2(struct cut_test *cut_test)
{
	cut_assert_int_neq(4, 5);
	cut_assert_int_neq(4, 4);
}

static void test3(struct cut_test *cut_test)
{
	cut_assert_int_range(0, 10, 4);
	cut_assert_int_range(0, 10, 100);
}

static void test4(struct cut_test *cut_test)
{
	void *ptr = NULL;

	cut_assert_mem_not_null(cut_test);
	cut_assert_mem_not_null(ptr);
}

static void test5(struct cut_test *cut_test)
{
	void *ptr = NULL;

	cut_assert_mem_null(ptr);
	cut_assert_mem_null(cut_test);
}

int main(int argc, char *argv[])
{
	struct cut_test tests[] = {
		cut_test(set_up, tear_down, test1, NULL),
		cut_test(set_up, tear_down, test2, NULL),
		cut_test(set_up, tear_down, test3, NULL),
		cut_test(set_up, tear_down, test4, NULL),
		cut_test(set_up, tear_down, test5, NULL),
	};
	struct cut_suite suite = cut_suite("cut main test suite\0",
					   tests, ARRAY_SIZE(tests),
					   NULL, NULL, NULL);

	cut_suite_init(&suite);
	cut_suite_run(&suite, CUT_VERBOSE);

	return 0;
}
