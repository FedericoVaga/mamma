/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "cut.h"

/**
 * It creates a test suite, in other words a collection of tests.
 *
 * @param[in] name suite name
 * @return a valid pointer to the test suite
 */
struct cut_suite *cut_suite_create(char *name)
{
	struct cut_suite *cut_suite;

	cut_suite = malloc(sizeof(struct cut_suite));
	assert(cut_suite != NULL); /* abort if we cannot allocate */
	memset(cut_suite, 0, sizeof(struct cut_suite));
	strncpy(cut_suite->name, name, CUT_NAME_MAX_LEN);

	return cut_suite;
}


/**
 * It destory a test suite created with cut_suite_create()
 * @param[in] cut_suite the test suite to destory
 */
void cut_suite_destroy(struct cut_suite *cut_suite)
{
	free(cut_suite);
}
