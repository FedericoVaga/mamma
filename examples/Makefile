PROGRAMS := cut_test

CFLAGS := -Wall -Werror -ggdb -O2 -I$(CUT)/lib $(EXTRACFLAGS)
LDFLAGS := -L$(CUT)/lib -lcut

all: $(PROGRAMS)

clean:
	$(RM) $(PROGRAMS) *.o *~

%: %.c
	$(CC) $(CFLAGS) $*.c $(LDFLAGS)  -o $@

.PHONY: all clean
