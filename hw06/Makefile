CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -g -I. -MMD -MP
LDLIBS = -lm

MODULE_OBJS = sensor.o panel.o scenario.o iom361_r4.o
REPLAY_OBJS = main.o $(MODULE_OBJS)
HW06_TEST_OBJS = tests/test_hw06.o $(MODULE_OBJS)
SCENARIO_TEST_OBJS = tests/test_scenario.o scenario.o

.PHONY: all test clean

all: replay

replay: $(REPLAY_OBJS)
	$(CC) $(REPLAY_OBJS) -o $@ $(LDLIBS)

tests/test_hw06: $(HW06_TEST_OBJS)
	$(CC) $(HW06_TEST_OBJS) -o $@ $(LDLIBS) -Wl,--wrap=iom361_readReg

tests/test_scenario: $(SCENARIO_TEST_OBJS)
	$(CC) $(SCENARIO_TEST_OBJS) -o $@ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: replay tests/test_hw06 tests/test_scenario
	./tests/test_hw06
	./tests/test_scenario
	sh regress.sh

clean:
	rm -f replay *.o *.d tests/*.o tests/*.d tests/test_hw06 tests/test_scenario tests/scenario_test_temp.txt

-include $(REPLAY_OBJS:.o=.d) tests/test_hw06.d tests/test_scenario.d
