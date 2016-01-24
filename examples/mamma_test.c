/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <mamma.h>

static void set_up(struct m_test *m_test)
{
}
static void tear_down(struct m_test *m_test)
{
}

static void test1(struct m_test *m_test)
{
	m_assert_int_eq(4, 4);
	m_assert_int_neq(4, 5);
	m_assert_int_gt(5, 4);
	m_assert_int_ge(5, 4);
	m_assert_int_ge(5, 5);
	m_assert_int_lt(4, 5);
	m_assert_int_le(4, 5);
	m_assert_int_le(5, 5);
	m_assert_int_range(0, 10, 4);
	m_assert_int_nrange(0, 10, 15);
	m_assert_dbl_eq(4.123456, 4.123456);
	m_assert_dbl_neq(4.21453523, 4.657454);
	m_assert_dbl_gt(5.12433522, 5.023545);
	m_assert_dbl_ge(5.12312, 4.1231);
	m_assert_dbl_ge(5.556, 5.556);
	m_assert_dbl_lt(4.34234, 4.93243);
	m_assert_dbl_le(4.143234, 4.54352);
	m_assert_dbl_le(5.123, 5.123);
	m_assert_dbl_range(0, 1, 0.12425435);
	m_assert_dbl_nrange(0, 1, 1.12312);
}

static void test2(struct m_test *m_test)
{
	m_check_int_eq(4, 5);
	m_check_int_neq(4, 4);
	m_check_int_gt(5, 6);
	m_check_int_ge(5, 6);
	m_check_int_ge(5, 6);
	m_check_int_lt(4, 3);
	m_check_int_le(4, 3);
	m_check_int_le(5, 3);
	m_check_int_range(0, 10, 10);
	m_check_int_nrange(0, 10, 5);
	m_check_dbl_eq(4.1234562, 4.123456);
	m_check_dbl_neq(4.214, 4.214);
	m_check_dbl_gt(5.12433522, 6.023545);
	m_check_dbl_ge(5.12312, 6.1231);
	m_check_dbl_ge(5.556, 5.5561);
	m_check_dbl_lt(4.34234, 3.93243);
	m_check_dbl_le(4.143234, 3.54352);
	m_check_dbl_le(5.1232, 5.1231);
	m_check_dbl_range(0, 1, 1.2425435);
	m_check_dbl_nrange(0, 1, 0.12312);
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
