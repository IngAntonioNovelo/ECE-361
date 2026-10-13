CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g
OBJS = main.o reading.o stats.o histogram.o
TEST_EXE = tests/test_stats

.PHONY: all test clean
all: readings

readings: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

main.o: main.c reading.h stats.h histogram.h
reading.o: reading.c reading.h
stats.o: stats.c stats.h
histogram.o: histogram.c histogram.h

$(TEST_EXE): tests/test_stats.c stats.o stats.h
	$(CC) $(CFLAGS) -I. -o $@ tests/test_stats.c stats.o

test: readings $(TEST_EXE)
	./$(TEST_EXE)
	sh regress.sh

clean:
	rm -f $(OBJS) readings $(TEST_EXE)
