/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
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

}

static void test3(struct m_test *m_test)
{
	int a = 4, b = 5;

	m_check_custom(a == b || a == 1, EINVAL,
		       "%d != %d and %d != 1", a, b, a);
	m_assert_custom(a == b || a == 1, EINVAL,
			"%d != %d and %d != 1", a, b, a);
	m_assert_custom(a == b || a == 1, EINVAL,
			"%d != %d and %d != 1", a, b, a);
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

/**
 * It uses all checks
 */
static void test_check_only(struct m_test *m_test)
{
	/* Integers */

	m_check_int_eq(4, 5); /* Err */
	m_check_int_eq(4, 4);

	m_check_int_neq(4, 5);
	m_check_int_neq(4, 4); /* Err */

	m_check_int_gt(5, 4);
	m_check_int_gt(5, 5); /* Err */
	m_check_int_gt(5, 6); /* Err */

	m_check_int_ge(5, 4);
	m_check_int_ge(5, 5);
	m_check_int_ge(5, 6); /* Err */

	m_check_int_lt(5, 6);
	m_check_int_lt(5, 5); /* Err */
	m_check_int_lt(5, 4); /* Err */

	m_check_int_le(5, 6);
	m_check_int_le(5, 5);
	m_check_int_le(5, 4); /* Err */

	m_check_int_range(0, 10, 11); /* Err */
	m_check_int_range(0, 10, 5);

	m_check_int_nrange(0, 10, 11);
	m_check_int_nrange(0, 10, 5); /* Err */

	/* Double */

	m_check_dbl_eq(4.123456, 4.123456);
	m_check_dbl_eq(4.123456, 4.123457); /* Err */

	m_check_dbl_neq(4.123456, 4.123456); /* Err */
	m_check_dbl_neq(4.123456, 4.123457);

	m_check_dbl_gt(4.123456, 4.123455);
	m_check_dbl_gt(4.123456, 4.123456); /* Err */
	m_check_dbl_gt(4.123456, 4.123457); /* Err */

	m_check_dbl_ge(4.123456, 4.123455);
	m_check_dbl_ge(4.123456, 4.123456);
	m_check_dbl_ge(4.123456, 4.123457); /* Err */

	m_check_dbl_lt(4.123456, 4.123457);
	m_check_dbl_lt(4.123456, 4.123456); /* Err */
	m_check_dbl_lt(4.123456, 4.123455); /* Err */

	m_check_dbl_le(4.123456, 4.123457);
	m_check_dbl_le(4.123456, 4.123456);
	m_check_dbl_le(4.123456, 4.123455); /* Err */

	m_check_dbl_range(0, 1, 1.2425435); /* Err */
	m_check_dbl_range(0, 1, 0.2425435);

	m_check_dbl_nrange(0, 1, 1.2425435);
	m_check_dbl_nrange(0, 1, 0.2425435); /* Err */

	/* Memory - use string to make it simple */

	m_check_mem_not_null(NULL); /* Err */
	m_check_mem_not_null("bbb");

	m_check_mem_null(NULL);
	m_check_mem_null("bbb"); /* Err */

	m_check_mem_eq("bbb", "bbb", 4);
	m_check_mem_eq("bbb", "aaa", 4); /* Err */

	m_check_mem_neq("bbb", "aaa", 4);
	m_check_mem_neq("bbb", "bbb", 4); /* Err */

	m_check_mem_gt("bbb", "aaa", 4);
	m_check_mem_gt("bbb", "bbb", 4); /* Err */
	m_check_mem_gt("bbb", "ccc", 4); /* Err */

	m_check_mem_ge("bbb", "aaa", 4);
	m_check_mem_ge("bbb", "bbb", 4);
	m_check_mem_ge("bbb", "ccc", 4); /* Err */

	m_check_mem_lt("bbb", "ddd", 4);
	m_check_mem_lt("bbb", "bbb", 4); /* Err */
	m_check_mem_lt("bbb", "aaa", 4); /* Err */

	m_check_mem_le("bbb", "ddd", 4);
	m_check_mem_le("bbb", "bbb", 4);
	m_check_mem_le("bbb", "aaa", 4); /* Err */

	m_check_mem_range("bbb", "ddd", "aaa", 4); /* Err */
	m_check_mem_range("bbb", "ddd", "ccc", 4);

	m_check_mem_nrange("bbb", "ddd", "aaa", 4);
	m_check_mem_nrange("bbb", "ddd", "ccc", 4);  /* Err */

	/* Strings */

	m_check_str_not_null(NULL); /* Err */
	m_check_str_not_null("bbb");

	m_check_str_null(NULL);
	m_check_str_null("bbb"); /* Err */

	m_check_str_eq("bbb", "bbb", 4);
	m_check_str_eq("bbb", "aaa", 4); /* Err */

	m_check_str_neq("bbb", "aaa", 4);
	m_check_str_neq("bbb", "bbb", 4); /* Err */

	m_check_str_gt("bbb", "aaa", 4);
	m_check_str_gt("bbb", "bbb", 4); /* Err */
	m_check_str_gt("bbb", "ccc", 4); /* Err */

	m_check_str_ge("bbb", "aaa", 4);
	m_check_str_ge("bbb", "bbb", 4);
	m_check_str_ge("bbb", "ccc", 4); /* Err */

	m_check_str_lt("bbb", "ddd", 4);
	m_check_str_lt("bbb", "bbb", 4); /* Err */
	m_check_str_lt("bbb", "aaa", 4); /* Err */

	m_check_str_le("bbb", "ddd", 4);
	m_check_str_le("bbb", "bbb", 4);
	m_check_str_le("bbb", "aaa", 4); /* Err */

	m_check_str_range("bbb", "ddd", "aaa", 4); /* Err */
	m_check_str_range("bbb", "ddd", "ccc", 4);

	m_check_str_nrange("bbb", "ddd", "aaa", 4);
	m_check_str_nrange("bbb", "ddd", "ccc", 4);  /* Err */
}

static void test_single_ko(struct m_test *m_test)
{
	m_assert_int_neq(4, 4);
	m_assert_int_neq(4, 5);
}


int main(int argc, char *argv[])
{
	struct m_test test_check = m_test(NULL, test_check_only, NULL);
	struct m_test test_sng_ko = m_test(NULL, test_single_ko, NULL);
	struct m_test tests[] = {
		m_test(set_up, test1, tear_down),
		m_test(NULL, test2, NULL),
		m_test(NULL, test3, NULL),
		m_test(NULL, test4, NULL),
		m_test(NULL, test5, NULL),
		m_test(NULL, test6, NULL),
	};
	struct m_suite suite = {
		.name = "cut main test suite\0",
		.flags = M_VERBOSE | M_ERRNO,
		.private = NULL,
		.tests = tests,
		.test_count = ARRAY_SIZE(tests),
		.set_up = NULL,
		.tear_down = NULL,
	};

	m_suite_run(&suite);
	m_test_run(&test_sng_ko);

	m_test_run(&test_check);

	return 0;
}
