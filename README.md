Mamma is a C Unit Test framework. *Mamma* in Italian means mom. The reason
behind this name is simple: a good mom always looks over her children behavior,
so as the unit-test looks over our code behavior in order to make as perfer as
possible.


### Why Yet Another C Unit-Test ?
The short answer is: one night I was bored :)

The long answer. I was using some other minimal C Unit-Test framework and, as an
user, I never had a problem. Honestly, I do not think that *mamma* is better
then the other unit-tests framework it is just comparable.
I wrote my own because it is always funny to do something by yourself
and because when I tried to modified some other framework I was not really
comfortable with the code: too complex sometimes for what it gives you. So, the
decision to write my own with two main objectives: keep it simple for the user
and keep it simple for the developer.


# How to Use Mamma?
As any other framework, there is a bit of infrastructure that you have to build
in order to be able to run your tests. This infrastructure consist in the test
declaration and, optionally, the test-suite declaration.
- code and run each test independently
```c
struct mytest = {...};
m_test_run(&mytest);
```

- code your tests, collect them in a suite and run the test suite
```c
struct mytests[] = {...};
struct m_suite suite = {
	...
	.test = mytests,
	.test_count = ARRAY_SIZE(mytests),
	...
};
m_suite_run(&suite);
```

In the example directory you can find a file named `skeleton.c` which contains
the basic structure.


# The Test

You can declare a test only by using the `m_test` data structure. Not all the
fields within this structure are important for the user. Following the ones
that you should care about:

```c
struct m_test {
	...
	void (*set_up)(struct m_test *test);
	void (*test)(struct m_test *test);
	void (*tear_down)(struct m_test *test);
	...
};
```

| Function      | Description                                                   |
| ------------- | ------------------------------------------------------------- |
| `set_up()`    | Prepare the environment to properly run the test              |
| `test()`      | The real test to run                                          |
| `tear_down()` | It undo what `set_up()` did in order to clean the environment |

All those function can use assertions without any limitation.

### Assertions
The heart of the test is the assertion. Mamma's `assert` stops the test
execution and it prints an error message that explain the reason. This framework
provides also a less rigurous assertion called `check`. Using a check, the
framework will not stop the test exectution, but it will print the error
message and continue the test execution (it can be useful for minor controls).
For each existent assertion it exists also its check version.

Following some examples; you can generate the doxygen documentation for the full
list of available assertions and checks.


| Command                                       | Description                                               |
| --------------------------------------------- | --------------------------------------------------------- |
| `m_assert_int_eq(int a, int b)`               | error if the condition *a == b* is not satified           |
| `m_assert_int_range(int min, int max, int a)` | error if the condition *min <= a <= max* is not satisfied |
| `m_assert_dbl_ge(int a, int b)`               | error if the condition *a > b* is not satisfied           |
| `m_assert_mem_not_null(void *ptr)`            | error if the condition *ptr != NULL* is not satisfied     |

### Context
If you need to exchange information from the `set_up()` to the `test()` or
`tear_down`, you can use the `m_test->private` pointer to store your data.
This data, typically, represent the test context. For instance, it may contains
file descriptors, libraries tokens and so on.

```c
void setup(struct m_test *t)
{
	...
	t->private = my_context;
	...
}

void test(struct m_test *t)
{
	...
	my_context = t->private;
	...
}

void teardown(struct m_test *t)
{
	...
	my_context = t->private;
	...
}
```

## The Suite
The test suite is nothing more than a collection of tests running within the
same context.

```c
struct m_suite {
	const char *name;
	struct m_test *tests;
	unsigned int test_count;
	void (*set_up)(struct m_suite *m_suite);
	void (*tear_down)(struct m_suite *m_suite);
	char *(*strerror)(int errnum);
	...
};
```

| Function      | Description                                                   |
| ------------- | ------------------------------------------------------------- |
| `name`        | Test suite name                                               |
| `tests`       | Array of tests to run within the suite                        |
| `test_count`  | Number of available tests                                     |
| `set_up()`    | Prepare the environment to properly run the test suite        |
| `tear_down()` | It undo what `set_up()` did in order to clean the environment |
| `strerror()`  | Custum implementation of strerror() to handle custum errno    |

All those function can use assertions without any limitation.

### Context
It is possible to define also a test suite context by using the variable
`m_suite->private`. The purpose is exactly the same as for tests, but extended
to a collection of tests. The suite context is accessible from a test using the
following pointers `m_test->m_suite->private`:

```c
void test(struct m_test *t)
{
	...
	context_test = t->private;
	context_suite = t->m_suite->private;
	...
}
```

## Examples
In the directory *examples* you can find a skeleton that you can use to start
your own test program and a test program example.


# Behind The Scene (For Contributors)
The mamma implementation is easy as its usage.
The entire test mechanism is based on a state machine that you can see in the
uml diagram available in `doc/uml/uml_mamma_state_machine.png`

![State Machine](/doc/uml/uml_mamma_state_machine.png)

This state machine is based on function callbacks (representing the states in
the diagram) and long-jump (representing the arrows in the diagram). Each
callback knows which are the next possible states and it uses a long-jump to do
the transition to the next state.

I'm using the callback way (instead of switch case) because I think is better
and the code cleanery. I'm using long-jump to go from una state to another state
because it is **necessary** to jump out from an error condition; so instead of
having a mixed transition system I prefer to have an unifor way to do
transitions between states.

### Assertions and Checks
Internally all assertions and checks are handled by the same function
`m_check()` that uses variad parameters to be able to accept different
parameters according to the the assertion/check type.
All assertions/checks are macros; those macros properly set the `m_check`
parameters.

It is possible to use a single function to handle all assertions and checks
because all these assertions and checks are based on an array of the following
data structure:

```c
struct m_assertion {
	int (*condition)();
	const char *fmt;
	const unsigned int errorno;
};
```




### State Machine Implementation
We can see how simple is the state machine by looking at its implementation:

```c
static void m_suite_run_state_machine(struct m_suite *m_suite)
{
	m_suite_cur = m_suite;
	errno = 0;

	state_cur = setjmp(global_jbuf);
	assert(state_cur < _M_STATE_MAX);
	state_machine[state_cur]();
}
```

Each `longjmp` call makes the code jump to the `setjmp` line above. The `setjmp`
return value represent the next callback (state) to execute. To make the things
a bit more clear, we can take a look also at a callback (state) implementation.

```c
static void m_state_go_to(enum m_state_machine state)
{
	state_prv = state_cur;
	longjmp(global_jbuf, state);
}

static void m_state_suite_set_up(void)
{
	if (m_suite_cur->set_up)
		m_suite_cur->set_up(m_suite_cur);

	m_test_cur = &m_suite_cur->tests[0];

	m_state_go_to(M_STATE_TEST_SET_UP);
}
```
