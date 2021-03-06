/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
static const char *test_good_desc = "It uses all the asserts functions provided by the framework in order to test them";

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
static const char *test_check_desc = "It uses all the checks functions provided by the framework in order to test them";


static void test_good_real_func(struct m_test *m_test)
{
	int fd;

	fd = open("file-that-do-not-exist", O_RDONLY);
	m_check_int_eq(-1, fd);
	m_check_errno_eq(ENOENT);
	m_assert_errno_eq(ENOENT);
}
static const char *test_good_real_func_desc = "It uses a real function that fails";

static void test_bad_real_func(struct m_test *m_test)
{
	int fd;

	fd = open("file-that-do-not-exist-but-we-think-it-does", O_RDONLY);
	m_check_int_neq(-1, fd);
	m_check_errno_neq(ENOENT);
	m_assert_errno_neq(ENOENT);
}
static const char *test_bad_real_func_desc = "It uses a real function that fails but we make the assumption that it does not";


int main(int argc, char *argv[])
{
	struct m_test tests[] = {
		m_test_desc(NULL, test_good_assert, NULL,
			    test_good_desc),
		m_test_desc(NULL, test_check_only, NULL,
			    test_check_desc),
		m_test_desc_loop(NULL, test_good_assert, NULL,
				 test_good_desc, 10),
		m_test_desc_loop(NULL, test_check_only, NULL,
				 test_check_desc, 10),
		m_test_desc(NULL, test_good_real_func, NULL,
			    test_good_real_func_desc),
		m_test_desc(NULL, test_bad_real_func, NULL,
			    test_bad_real_func_desc),
	};
	struct m_suite suite = {
		.name = "Mamma auto-test",
		.desc = "It tests all the mamma features",
		.flags = M_VERBOSE | M_ERRNO_CHECK | M_ERRNO_FUNC,
		.private = NULL,
		.tests = tests,
		.test_count = M_ARRAY_SIZE(tests),
		.set_up = NULL,
		.tear_down = NULL,
		.strerror = NULL,
	};

	m_suite_run(&suite);

	assert(0 == tests[0].warnings);
	assert(M_STATE_EXIT_SUCCESS == tests[0].exit);
	assert(44 == tests[1].warnings);
	assert(M_STATE_EXIT_SUCCESS == tests[0].exit);

	return 0;
}
