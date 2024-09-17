CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
GCOV:=$(FLAGS) -coverage
OBJS = build/maze.o build/backend_maze.o build/cave.o build/ml_agent.o

all: $(OBJS)
	$(CC) $(CFLAGS) $^ -o maze -lncursesw -lgcov -lm

lib_maze.a: build/backend_maze.o
	ar rc $@ build/backend_maze.o
	ranlib lib_maze.a

lib_maze_plus.a: $(OBJS)
	ar rc $@ $(OBJS)
	ranlib lib_maze_plus.a

install: $(OBJS)
	make
	mkdir ~/Maze_by_s21/
	install ./maze ~/Maze_by_s21/

uninstall:
	rm -rf ~/Maze_by_s21/

tests: clean
	$(CC) $(CFLAGS) backend_maze.c cave.c ml_agent.c tests/*.c -o test -lcheck -lm -lsubunit -L. -lncurses -pthread
	./test

gcov_report: clean extra_flags lib_maze.a tests
	-rm -rf report/
	-lcov --rc lcov_branch_coverage=1 -t "a1_maze_report" -o a1_maze_report.info -c -d .
	-lcov --rc lcov_branch_coverage=1 -o a1_maze_report.info -r a1_maze_report.info "*/tests/*"
	-genhtml --rc lcov_branch_coverage=1 -o report a1_maze_report.info
	-rm -rf *.gcno *.gcda *.info out/*.gcno

clean:
	rm -rf build test maze report/ *.a *.h.gch

build: $(OBJS)
	$(CC) $(CFLAGS) $^ -o maze -lncursesw -lgcov -lm
	./maze

build/%.o: %.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clang:
	clang-format -i *.c *.h tests/*.c tests/*.h

clang_check:
	clang-format -n *.c *.h tests/*.c tests/*.h

valgrind: tests
	valgrind --tool=memcheck --leak-check=yes ./test

check: all
	cppcheck ./*.c

extra_flags:
	$(eval CFLAGS:=$(GCOV))

.PHONY: all, install, uninstall, tests, gcov_report, clean, build, clang, clang_check, valgrind, check