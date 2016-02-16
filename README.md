Mamma is a C Unit Test framework. *Mamma* in Italian means mom. The reason
behind this name is simple: a good mom always looks over her children behavior,
so as the unit-test looks over our code behavior in order to make as perfer as
possible.

The aim of this README file is to provide you an overview of the project.
For any details, please, generate the doxygen documentation.

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

# Documentation
The API documentation can be generated using doxygen. Run the following
commands in order to generate it:

```sh
cd doc
make doxygen
```

# Architecure
Mamma's architecure is based on the [Kent Back SUnit](https://web.archive.org/web/20150315073817/http://www.xprogramming.com/testfram.htm) one.
The ![skeleton.c](/examples/skeleton.c) file can be used as starting point.
It contains the basic configuration to start the development of your test
program. To undersantd what each function/structure does, take a look also to
the doxygen documentation.

# Assertions
The heart of the test is the assertion. Mamma's `assert` stops the test
execution and it prints an error message that explain the reason. This framework
provides also a less rigurous assertion called `check`. Using a `check`, the
framework will **not** stop the test exectution; it will print the error
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

# Context
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

It is aslo possible to define a test suite context by using the variable
`m_suite->private`. The purpose is exactly the same as for tests, but extended
to the test-suite. The suite context is accessible from a test using the
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


# Behind The Scene (For Contributors)
## State Machine
The mamma implementation is easy as its usage.
The entire test mechanism is based on a state machine that you can see in the
uml diagram visible on picture `doc/uml/uml_mamma_state_machine.png`

![State Machine](/doc/uml/uml_mamma_state_machine.png)

This state machine is based on function callbacks (representing the states in
the diagram) and long-jump (representing the arrows in the diagram). Each
callback knows which are the next possible states and it uses a long-jump to do
the transition to the next state.

I'm using the callback way (instead of switch case) because I think is better
and the code cleaner. I'm using long-jump to go from una state to another state
because it is **necessary** to jump out from an error condition; so instead of
having a mixed transition system I prefer to have an unifor way to do
transitions between states.

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
return value represent the next callback (state) to execute. To make things
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


## Assertions and Checks
Internally all assertions and checks are handled by the same function
`m_check()` that uses variadic parameters to be able to accept different
parameters according tothe assertion/check type.
All assertions/checks are macros; those macros properly set the `m_check`
parameters. For example:

```c
#define m_assert_int_eq(_exp, _val)			\
	m_check(M_INT_EQ, M_FLAG_STOP_ON_ERROR,		\
		(__func__), (__LINE__), (_exp), (_val))
#define m_check_int_eq(_exp, _val)			\
	m_check(M_INT_EQ, M_FLAG_CONT_ON_ERROR,		\
		(__func__), (__LINE__), (_exp), (_val))
#define m_check_dbl_le(_exp, _val)		\
	m_check(M_DBL_LE, M_FLAG_CONT_ON_ERROR,	\
		(__func__), (__LINE__),		\
		(double)(_exp), (double)(_val))
```

Internally, mamma uses an enumerated array of condition tests. Using the proper
condition number (e.g. M_INT_EQ, M_DBL_LE) it is possible to run the associated
test function. Each condition type is described using the following structure:

```c
struct m_assertion {
	int (*condition)();
	const char *fmt;
	const unsigned int errorno;
};
```
