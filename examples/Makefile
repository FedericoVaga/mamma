MAMMA ?= ../

PROGRAMS := mamma_test
PROGRAMS += skeleton

CFLAGS := -Wall -Werror -ggdb -O2 -I$(MAMMA)/lib $(EXTRACFLAGS)
LDFLAGS := -L$(MAMMA)/lib -lmamma

all: $(PROGRAMS)

clean:
	$(RM) $(PROGRAMS) *.o *~

%: %.c
	$(CC) $(CFLAGS) $*.c $(LDFLAGS)  -o $@

.PHONY: all clean
