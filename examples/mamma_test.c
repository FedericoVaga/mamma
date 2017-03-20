/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <mamma.h>


/**
 * It uses all checks
 */
static void test_good_assert(struct m_test *m_test)
{
	/* Integers */

	m_assert_int_eq(4, 4);

	m_assert_int_neq(4, 5);

	m_assert_int_gt(5, 4);

	m_assert_int_ge(5, 4);
	m_assert_int_ge(5, 5);

	m_assert_int_lt(5, 6);

	m_assert_int_le(5, 6);
	m_assert_int_le(5, 5);

	m_assert_int_range(0, 10, 5);

	m_assert_int_nrange(0, 10, 11);

	/* Double */

	m_assert_dbl_eq(4.123456, 4.123456);

	m_assert_dbl_neq(4.123456, 4.123457);

	m_assert_dbl_gt(4.123456, 4.123455);

	m_assert_dbl_ge(4.123456, 4.123455);
	m_assert_dbl_ge(4.123456, 4.123456);

	m_assert_dbl_lt(4.123456, 4.123457);

	m_assert_dbl_le(4.123456, 4.123457);
	m_assert_dbl_le(4.123456, 4.123456);

	m_assert_dbl_range(0, 1, 0.2425435);

	m_assert_dbl_nrange(0, 1, 1.2425435);

	/* Memory - use string to make it simple */

	m_assert_mem_not_null("bbb");

	m_assert_mem_null(NULL);

	m_assert_mem_eq("bbb", "bbb", 4);

	m_assert_mem_neq("bbb", "aaa", 4);

	m_assert_mem_gt("bbb", "aaa", 4);

	m_assert_mem_ge("bbb", "aaa", 4);
	m_assert_mem_ge("bbb", "bbb", 4);

	m_assert_mem_lt("bbb", "ddd", 4);

	m_assert_mem_le("bbb", "ddd", 4);
	m_assert_mem_le("bbb", "bbb", 4);

	m_assert_mem_range("bbb", "ddd", "ccc", 4);

	m_assert_mem_nrange("bbb", "ddd", "aaa", 4);

	/* Strings */

	m_assert_str_not_null("bbb");

	m_assert_str_null(NULL);

	m_assert_str_eq("bbb", "bbb", 4);

	m_assert_str_neq("bbb", "aaa", 4);

	m_assert_str_gt("bbb", "aaa", 4);

	m_assert_str_ge("bbb", "aaa", 4);
	m_assert_str_ge("bbb", "bbb", 4);

	m_assert_str_lt("bbb", "ddd", 4);

	m_assert_str_le("bbb", "ddd", 4);
	m_assert_str_le("bbb", "bbb", 4);

	m_assert_str_range("bbb", "ddd", "ccc", 4);

	m_assert_str_nrange("bbb", "ddd", "aaa", 4);
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


int main(int argc, char *argv[])
{
		struct m_test tests[] = {
		m_test(NULL, test_good_assert, NULL),
		m_test(NULL, test_check_only, NULL),
	};
	struct m_suite suite = {
		.name = "Mamma auto-test",
		.flags = M_VERBOSE | M_ERRNO,
		.private = NULL,
		.tests = tests,
		.test_count = M_ARRAY_SIZE(tests),
		.set_up = NULL,
		.tear_down = NULL,
	};

	m_suite_run(&suite);

	assert(0 == tests[0].warnings);
	assert(M_STATE_EXIT_SUCCESS == tests[0].exit);
	assert(44 == tests[1].warnings);
	assert(M_STATE_EXIT_SUCCESS == tests[0].exit);

	return 0;
}
