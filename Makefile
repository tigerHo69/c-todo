CC      = cc
CFLAGS  = -std=c17 -Wall -Wextra -Wpedantic -O2
LDFLAGS = -lsqlite3

SRC     = src/main.c src/cli.c src/db.c src/task.c
OBJ     = $(SRC:.c=.o)
BIN     = todo

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(BIN) tests/test_db tests/test_cli

test: tests/test_db tests/test_cli
	./tests/test_db
	./tests/test_cli

tests/test_db: tests/test_db.c src/db.c src/task.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

tests/test_cli: tests/test_cli.c src/cli.c src/task.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: all clean test
