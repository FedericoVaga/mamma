/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <cut.h>

static void set_up(struct cut_test *cut_test)
{
	printf("Setting up test\n");
}
static void tear_down(struct cut_test *cut_test)
{
	printf("Clearning test\n");
}

static void test1(struct cut_test *cut_test)
{
	printf("doing something\n");
}

static void test2(struct cut_test *cut_test)
{
	printf("doing something else\n");
}

int main(int argc, char *argv[])
{
	struct cut_test tests[] = {
		cut_test_declare("first", set_up, tear_down, test1, NULL),
		cut_test_declare("second", set_up, tear_down, test2, NULL),
	};
	struct cut_suite suite = cut_suite_declare("cut main test suite",
						   tests, ARRAY_SIZE(tests),
						   NULL);

	cut_suite_init(&suite);
	cut_suite_run(&suite, CUT_VERBOSE);

	return 0;
}
