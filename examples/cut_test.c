/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <cut.h>

static void set_up(struct m_test *m_test)
{
}
static void tear_down(struct m_test *m_test)
{
}

static void test1(struct m_test *m_test)
{
	m_assert_int_eq(4, 4);
	m_assert_int_eq(4, 5);
}

static void test2(struct m_test *m_test)
{
	m_assert_int_neq(4, 5);
	m_assert_int_neq(4, 4);
}

static void test3(struct m_test *m_test)
{
	m_assert_int_range(0, 10, 4);
	m_assert_int_range(0, 10, 100);
}

static void test4(struct m_test *m_test)
{
	void *ptr = NULL;

	m_assert_mem_not_null(m_test);
	m_assert_mem_not_null(ptr);
}

static void test5(struct m_test *m_test)
{
	void *ptr = NULL;

	m_assert_mem_null(ptr);
	m_assert_mem_null(m_test);
}

static void test6(struct m_test *m_test)
{
	m_assert_int_neq(4, 5);
	m_assert_int_neq(4, 6);
	m_skip(4 == 5);
	m_assert_int_neq(4, 7);
	m_skip(5 == 5);
	m_assert_int_neq(4, 4);
	m_assert_int_neq(4, 5);
	m_assert_int_neq(4, 4);
}

int main(int argc, char *argv[])
{
	struct m_test tests[] = {
		m_test(set_up, tear_down, test1, NULL),
		m_test(set_up, tear_down, test2, NULL),
		m_test(set_up, tear_down, test3, NULL),
		m_test(set_up, tear_down, test4, NULL),
		m_test(set_up, tear_down, test5, NULL),
		m_test(set_up, tear_down, test6, NULL),
	};
	struct m_suite suite = m_suite("cut main test suite\0",
					   tests, ARRAY_SIZE(tests),
					   NULL, NULL, NULL);

	m_suite_init(&suite);
	m_suite_run(&suite, M_VERBOSE | M_ERRNO);

	return 0;
}
