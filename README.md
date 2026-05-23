# todo

A fast, minimal command-line task manager written in C. Uses SQLite for reliable storage in a single file.

## Build

Requires a C17 compiler and SQLite3 development headers (included on macOS and most Linux distros).

```bash
make
```

This produces a `todo` binary in the project root. Optionally install it:

```bash
sudo cp todo /usr/local/bin/
```

### Build from source on Linux (if SQLite headers aren't installed)

```bash
# Debian/Ubuntu
sudo apt install libsqlite3-dev

# Fedora/RHEL
sudo dnf install sqlite-devel

make
```

## Usage

Running `todo` with no arguments lists pending tasks.

### Add a task

```bash
todo add "Buy groceries"
todo add "Submit report" -p high -d 2026-06-01
todo add "Read a book" -p low
```

**Flags:**
- `-p <priority>` — `high` (or `h`), `med` (or `m`), `low` (or `l`). Default: `med`
- `-d <date>` — due date in `YYYY-MM-DD` format

### List tasks

```bash
todo              # pending tasks (default)
todo list         # same as above
todo list --all   # pending + completed
todo list --done  # completed only
```

Tasks are sorted by priority (high first), then by due date (soonest first).

### Complete / undo

```bash
todo done 3       # mark task #3 as complete
todo undo 3       # reopen task #3
```

### Edit a task

```bash
todo edit 3 "New description"
todo edit 3 -p high
todo edit 3 -d 2026-07-01
todo edit 3 -d none              # clear the due date
todo edit 3 "Updated" -p low -d 2026-08-15
```

Only the fields you specify are changed; everything else stays the same.

### Delete a task

```bash
todo rm 3
```

### Search

```bash
todo search "groceries"
```

### Clear completed tasks

```bash
todo clear
```

Permanently removes all completed tasks from the database.

### Help

```bash
todo help
```

## Storage

Tasks are stored in a SQLite database at `~/.todo.db`. Override with the `TODO_DB` environment variable:

```bash
export TODO_DB=~/projects/work-tasks.db
todo add "Work thing"
```

## Environment Variables

| Variable | Description |
|----------|-------------|
| `TODO_DB` | Path to the SQLite database file (default: `~/.todo.db`) |
| `NO_COLOR` | Disable colored terminal output when set to any value |

## Running Tests

```bash
make test
```

## Project Structure

```
c-todo/
├── Makefile
├── src/
│   ├── main.c          Entry point and command dispatch
│   ├── cli.c / cli.h   Argument parsing
│   ├── db.c / db.h     SQLite operations
│   ├── task.c / task.h  Task display and formatting
│   └── color.h         ANSI color support
└── tests/
    ├── test_db.c       Database operation tests
    └── test_cli.c      CLI parsing tests
```

## License

MIT
