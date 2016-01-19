/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <cut.h>

static inline void prova(void)
{
	printf("%s ----\n", __func__);
}

static void set_up(struct cut_test *cut_test)
{
}
static void tear_down(struct cut_test *cut_test)
{
}

static void test1(struct cut_test *cut_test)
{
	cut_assert_int_eq(cut_test, 4, 4);
	cut_assert_int_eq(cut_test, 4, 5);
}

static void test2(struct cut_test *cut_test)
{
	prova();
	cut_assert_int_neq(cut_test, 4, 5);
	cut_assert_int_neq(cut_test, 4, 4);
}

static void test3(struct cut_test *cut_test)
{
	cut_assert_int_range(cut_test, 0, 10, 4);
	cut_assert_int_range(cut_test, 0, 10, 100);
}

int main(int argc, char *argv[])
{
	struct cut_test tests[] = {
		cut_test_declare("first\0", set_up, tear_down, test1, NULL),
		cut_test_declare("second\0", set_up, tear_down, test2, NULL),
		cut_test_declare("terzo\0", set_up, tear_down, test3, NULL),
	};
	struct cut_suite suite = cut_suite_declare("cut main test suite\0",
						   tests, ARRAY_SIZE(tests),
						   NULL, NULL,
						   NULL);

	cut_suite_init(&suite);
	cut_suite_run(&suite, CUT_VERBOSE);

	return 0;
}
