/**
 * Copyright 2015 Federico Vaga <www.federicovaga.com>
 */

#include <cut.h>


int main(int argc, char *argv[])
{
	struct cut_suite *suite;

	suite = cut_suite_create("cut main test suite");

	cut_suite_destroy(suite);

	return 0;
}
