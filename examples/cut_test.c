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
	cut_assert(cut_test, 2+2==5, "expected 4", __func__, __LINE__);
}

static void test2(struct cut_test *cut_test)
{
	cut_assert(cut_test, 2+2==4, "expected 4", __func__, __LINE__);
}

int main(int argc, char *argv[])
{
	struct cut_test tests[] = {
		cut_test_declare("first\0", set_up, tear_down, test1, NULL),
		cut_test_declare("second\0", set_up, tear_down, test2, NULL),
	};
	struct cut_suite suite = cut_suite_declare("cut main test suite\0",
						   tests, ARRAY_SIZE(tests),
						   NULL, NULL,
						   NULL);

	cut_suite_init(&suite);
	cut_suite_run(&suite, CUT_VERBOSE);

	return 0;
}
