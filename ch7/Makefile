ROOT=..
include $(ROOT)/Makefile

PROGS =	$(patsubst %.c,%,$(wildcard *.c))

all:	$(PROGS)

%:	%.c $(LIBAPUE)
	$(CC) $(CFLAGS) $@.c -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(PROGS) $(TEMPFILES) *.o
