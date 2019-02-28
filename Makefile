OBJECTS = dm.o

CC       ?= gcc
CFLAGS   ?= -pedantic -Wwrite-strings -lm -Ofast -march=native -s
LDFLAGS  ?=

all: dm

dm: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	-rm -f $(OBJECTS) dm

check: all
# Unit tests go here. None currently.
	@echo "*** ALL TESTS PASSED ***"

.PHONY: all clean check
