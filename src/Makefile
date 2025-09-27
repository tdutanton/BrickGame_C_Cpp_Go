SHELL = /bin/sh

MAIN_NAME = BrickGame_v3

CC = gcc -std=c11
CPPC = g++ -std=c++17
GO = go
FLAGS = -Wall -Werror -Wextra
GOFLAGS = -ldflags="-s -w" -tags="cgo"
NCUR = -lncurses
LFLAGS = -lcheck -lm -lsubunit
GCOV_FLAGS = -fprofile-arcs -ftest-coverage
TEST_FILE_NAME_T = test_tetris
TEST_FILE_NAME_S = test_snake
LIB_NAME_SNAKE = s21_snake.a
LIB_NAME_TETRIS = s21_tetris.a
LIB_NAME_RACE_SO = s21_race.so
SO_FLAG = -buildmode=c-shared
GCOV_NAME_T = gcov_tests_tetris.info
GCOV_NAME_S = gcov_tests_snake.info
GAME_NAME = ./s21_brick_game
MAIN_FILE_CLI = ./brick_game/s21_brick_game_cli.c

LIB_SRC_C = $(wildcard brick_game/tetris/*.c)
LIB_SRC_S = $(wildcard brick_game/snake/*.cpp)
FRONT_SRC = $(wildcard gui/cli/*.c)
TEST_SRC = $(wildcard tests/*.c)
TEST_SRC_S = $(wildcard tests/*.cpp)

LIB_RACE_GO = brick_game/race/s21_backend_race.go

RACE_WEB = race_web_game
TETRIS_WEB = tetris_web_game
SNAKE_WEB = snake_web_game

LIB_O_C = $(LIB_SRC_C:.c=.o)
LIB_O_CPP = $(LIB_SRC_S:.cpp=.o)
TEST_O = $(TEST_SRC:.c=.o)
TEST_O_CPP = $(TEST_SRC:.cpp=.o)

.PHONY: all install uninstall clean dvi dist tests \
		test_tetris test_snake gcov_report clang_check clang_fix \
		valgrind s21_tetris.a s21_snake.a valgrind dvi_install \
		run_race_web run_tetris_web run_snake_web \
		build_race_web build_tetris_web build_snake_web

all: clean install

help:
	@echo "Available targets:"
	@echo "  all          - clean, install"
	@echo "  install      - build all games"
	@echo "  run          - run main menu"
	@echo "  snake        - build snake"
	@echo "  tetris       - build tetris"
	@echo "  race         - build race (Go)"
	@echo "  tests        - run all tests"
	@echo "  gcov_report  - generate coverage report"
	@echo "  dist         - create distribution archive"
	@echo "  clean        - remove build artifacts"
	@echo "  go_linters   - run Go linters"

clean:
	@rm -f $(LIB_NAME_SNAKE) $(LIB_NAME_TETRIS) $(LIB_NAME_RACE_SO) $(TEST_FILE_NAME)
	@rm -rf ./brick_game/tetris/*.o ./brick_game/snake/*.o ./tests/*.o *.gcno *.gcda ./report *.o ./*.a gui/desktop/*.o
	@rm -rf menu snake tetris race
	@rm -rf s21_race.h
	@rm -rf $(TEST_FILE_NAME_T) $(TEST_FILE_NAME_S)
	@rm -rf html/
	@rm -rf latex/
	@rm -rf $(GAME_NAME).tgz
	@rm -rf $(GAME_NAME)
	@rm -rf gui/desktop/moc_* gui/desktop/Makefile.* gui/desktop/.qmake.* gui/desktop/ui_mainwindow.h
	@rm -rf report test
	@rm -rf gui/desktop/s21_snake_desktop gui/desktop/s21_tetris_desktop
	@rm -rf documentation
	@rm -f race.test race_high_score.txt
	@rm -f snake_high_score.txt tetris_high_score.txt brick_game/race/race_high_score.txt
	@rm -f gui/desktop/s21_race_desktop
	@rm -f $(TETRIS_WEB) $(SNAKE_WEB) $(RACE_WEB)

install: tetris snake race
	@$(CPPC) $(FLAGS) main_menu.cpp -o $(GAME_NAME)


run:
	@./$(GAME_NAME)

snake: $(LIB_NAME_SNAKE)
	@$(CPPC) $(FLAGS) -DSNAKE $(MAIN_FILE_CLI) $(FRONT_SRC) -o snake -lsubunit -L. -l:$(LIB_NAME_SNAKE) $(NCUR)
	@cd gui/desktop && qmake DEFINES+=SNAKE && make clean && make
	@mv gui/desktop/s21_brickgame_desktop gui/desktop/s21_snake_desktop
	@make build_snake_web


tetris: $(LIB_NAME_TETRIS)
	@$(CC) $(FLAGS) -DTETRIS $(MAIN_FILE_CLI) $(FRONT_SRC) -o tetris -lsubunit -L. -l:$(LIB_NAME_TETRIS) $(NCUR)
	@cd gui/desktop && qmake DEFINES+=TETRIS && make clean && make
	@mv gui/desktop/s21_brickgame_desktop gui/desktop/s21_tetris_desktop
	@make build_tetris_web

race: $(LIB_NAME_RACE_SO)
	@$(CC) $(FLAGS) -DRACE $(MAIN_FILE_CLI) $(FRONT_SRC) -o race -lsubunit -L. -l:$(LIB_NAME_RACE_SO) $(NCUR) -Wl,-rpath,$(PWD)
	@make build_race_web
	@make race_desktop

race_desktop:
	@cd gui/desktop && qmake DEFINES+=RACE && make clean && make && mv s21_brickgame_desktop s21_race_desktop

uninstall: clean
	-rm -rf $(GAME_NAME) snake_high_score.txt tetris_high_score.txt race_high_score.txt documentation

$(LIB_NAME_SNAKE): $(LIB_O_CPP)
	@ar rc $(LIB_NAME_SNAKE) $(LIB_O_CPP)
	@ranlib $(LIB_NAME_SNAKE)
	@rm -rf lib/*.o

$(LIB_NAME_TETRIS): $(LIB_O_C)
	@ar rc $(LIB_NAME_TETRIS) $(LIB_O_C)
	@ranlib $(LIB_NAME_TETRIS)
	@rm -rf lib/*.o

$(LIB_NAME_RACE_SO):
	@go build $(SO_FLAG) -o $(LIB_NAME_RACE_SO) $(GOFLAGS) $(LIB_RACE_GO)

%.o: %.c
	@$(CC) $(FLAGS) -c $< -o $@

%.o: %.cpp
	@$(CPPC) $(FLAGS) -c $< -o $@

tests: test_tetris test_snake test_race

test_tetris: $(TEST_O) $(LIB_NAME_TETRIS)
	@echo "Testing Tetris"
	@$(CC) $(FLAGS) $(TEST_O) -o $(TEST_FILE_NAME_T) -L. -l:$(LIB_NAME_TETRIS) $(LFLAGS)
	@./$(TEST_FILE_NAME_T)

test_snake: $(TEST_O) $(LIB_NAME_SNAKE)
	@echo "Testing Snake"
	@$(CPPC) $(FLAGS) -DTEST $(LIB_SRC_S) $(TEST_SRC_S)  -o $(TEST_FILE_NAME_S) -lm -lpthread -lgtest -pthread -lstdc++
	@./$(TEST_FILE_NAME_S)

test_race:
	@echo "Testing Race..."
	@go test -v -cover ./...

gcov_report: gcov_report_tetris gcov_report_snake

run_race_web:
	go run -tags=race_game ./brick_game/server/cmd

run_tetris_web:
	go run -tags=tetris ./brick_game/server/cmd

run_snake_web:
	go run -tags=snake ./brick_game/server/cmd

build_race_web:
	go build -tags=race_game -o $(RACE_WEB) ./brick_game/server/cmd

build_tetris_web:
	go build -tags=tetris -o $(TETRIS_WEB) ./brick_game/server/cmd

build_snake_web:
	go build -tags=snake -o $(SNAKE_WEB) ./brick_game/server/cmd

gcov_report_tetris: clean
	$(CC) $(FLAGS) -c $(LIB_SRC_C) --coverage
	$(CC) $(FLAGS) -c $(TEST_SRC) 
	$(CC) *.o -o $(TEST_FILE_NAME_T) $(LFLAGS) $(GCOV_FLAGS)
	./$(TEST_FILE_NAME_T)
	@lcov -t "gcov_tests" -o $(GCOV_NAME_T) -c -d .
	@genhtml -o report $(GCOV_NAME_T)
	@rm -rf *.o *.gcno *.gcda *.gcov $(GCOV_NAME_T)
	@rm -rf $(TEST_FILE_NAME_T)

gcov_report_snake: clean
	$(CPPC) $(FLAGS) -c $(LIB_SRC_S) --coverage
	$(CPPC) $(FLAGS) -c $(TEST_SRC_S) 
	$(CPPC) *.o -o $(TEST_FILE_NAME_S) -lgtest $(GCOV_FLAGS)
	./$(TEST_FILE_NAME_S)
	@lcov -t "gcov_tests" -o $(GCOV_NAME_S) -c -d . \
		--exclude '/usr/include/c++/11/*' \
		--exclude '/usr/include/c++/11/bits/*' \
		--exclude '/usr/include/c++/11/ext/*' \
		--exclude '/usr/include/gtest/*' \
		--exclude '/usr/include/gtest/internal/*'
	@genhtml -o report $(GCOV_NAME_S)
	@rm -rf *.gcno *.gcda *.gcov $(GCOV_NAME_S) *.o
	@rm -rf $(TEST_FILE_NAME_S)

valgrind: tests valgrind_race
	-valgrind --tool=memcheck --leak-check=yes ./test_tetris
	-valgrind --tool=memcheck --leak-check=yes ./test_snake

valgrind_race:
	@go test ./brick_game/race -c -o race.test
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=valgrind.supp ./race.test || true

dist: uninstall install
	mkdir -p $(MAIN_NAME)/gui/desktop
	cp $(GAME_NAME) $(MAIN_NAME)/
	cp -R gui/desktop/s21_snake_desktop $(MAIN_NAME)/gui/desktop/
	cp -R gui/desktop/s21_tetris_desktop $(MAIN_NAME)/gui/desktop/
	cp snake $(MAIN_NAME)/
	cp tetris $(MAIN_NAME)/
	cp race $(MAIN_NAME)/
	tar cvzf $(MAIN_NAME).tgz $(MAIN_NAME)/
	rm -rf $(MAIN_NAME)/

dvi:
	@mkdir documentation
	@doxygen Doxyfile
	@cd documentation/latex && xelatex refman.tex && cd ../../

dvi_install:
	sudo apt-get install texlive-xetex
	sudo apt-get install texlive-lang-cyrillic

go_linters:
	@echo "Running Go linters..."
	@$(GO) fmt ./...
	@$(GO) vet ./...
	@gofumpt -l -w ./... || true
	@golangci-lint run ./... --fix || true
	@echo "Go linting done."