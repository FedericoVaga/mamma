LIB := libcut.a
LIBS := libcut.so
LOBJ := cut.o

CFLAGS := -Wall -Werror -O2 -ggdb -fPIC $(EXTRACFLAGS)
LDFLAGS := -L. -lcut

all: $(LIB) $(LIBS)

clean:
	$(RM) $(LIB) $(LIBS) *~ *.o

%: %.c
	$(CC) $(CFLAGS) $*.c $(LDFLAGS) -o $@

$(LIB): $(LOBJ)
	$(AR) r $@ $^

$(LIBS): $(LIB)
	$(CC) -shared -o $@ -Wl,--whole-archive,-soname,$@ $^ -Wl,--no-whole-archive

.PHONY: clean all
