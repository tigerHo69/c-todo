#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "task.h"
#include "db.h"

static int parse_id(const char *s) {
    char *end;
    errno = 0;
    long val = strtol(s, &end, 10);
    if (errno != 0 || *end != '\0' || val <= 0 || val > INT_MAX)
        return -1;
    return (int)val;
}

static int parse_flags(int argc, char **argv, int start, cli_args_t *args) {
    for (int i = start; i < argc; i++) {
        if ((strcmp(argv[i], "-p") == 0) && i + 1 < argc) {
            args->priority = prio_from_str(argv[++i]);
            if (args->priority < 0) {
                fprintf(stderr, "error: invalid priority '%s' (use high, med, low)\n", argv[i]);
                return -1;
            }
        } else if ((strcmp(argv[i], "-d") == 0) && i + 1 < argc) {
            args->due = argv[++i];
        } else if (strcmp(argv[i], "--all") == 0) {
            args->filter = FILTER_ALL;
        } else if (strcmp(argv[i], "--done") == 0) {
            args->filter = FILTER_DONE;
        } else if (strcmp(argv[i], "--pending") == 0) {
            args->filter = FILTER_PENDING;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "error: unknown flag '%s'\n", argv[i]);
            return -1;
        } else if (!args->desc) {
            args->desc = argv[i];
        } else {
            fprintf(stderr, "error: unexpected argument '%s'\n", argv[i]);
            return -1;
        }
    }
    return 0;
}

int cli_parse(int argc, char **argv, cli_args_t *args) {
    memset(args, 0, sizeof(*args));
    args->priority = -1;

    if (argc < 2) {
        args->command = "list";
        return 0;
    }

    args->command = argv[1];

    if (strcmp(args->command, "add") == 0) {
        if (argc < 3) {
            fprintf(stderr, "usage: todo add <description> [-p priority] [-d due]\n");
            return -1;
        }
        args->desc = argv[2];
        return parse_flags(argc, argv, 3, args);
    }

    if (strcmp(args->command, "done") == 0 ||
        strcmp(args->command, "undo") == 0 ||
        strcmp(args->command, "rm") == 0) {
        if (argc < 3) {
            fprintf(stderr, "usage: todo %s <id>\n", args->command);
            return -1;
        }
        args->id = parse_id(argv[2]);
        if (args->id < 0) {
            fprintf(stderr, "error: invalid id '%s'\n", argv[2]);
            return -1;
        }
        return 0;
    }

    if (strcmp(args->command, "edit") == 0) {
        if (argc < 3) {
            fprintf(stderr, "usage: todo edit <id> [description] [-p priority] [-d due]\n");
            return -1;
        }
        args->id = parse_id(argv[2]);
        if (args->id < 0) {
            fprintf(stderr, "error: invalid id '%s'\n", argv[2]);
            return -1;
        }
        return parse_flags(argc, argv, 3, args);
    }

    if (strcmp(args->command, "search") == 0) {
        if (argc < 3) {
            fprintf(stderr, "usage: todo search <term>\n");
            return -1;
        }
        args->search = argv[2];
        return 0;
    }

    if (strcmp(args->command, "list") == 0) {
        return parse_flags(argc, argv, 2, args);
    }

    if (strcmp(args->command, "clear") == 0 ||
        strcmp(args->command, "help") == 0) {
        return 0;
    }

    fprintf(stderr, "error: unknown command '%s'\n", args->command);
    fprintf(stderr, "Run 'todo help' for usage.\n");
    return -1;
}

void cli_usage(void) {
    printf(
        "todo - a simple task manager\n"
        "\n"
        "Usage:\n"
        "  todo                          list pending tasks\n"
        "  todo add <desc> [-p pri] [-d due]   add a task\n"
        "  todo list [--all|--done]      list tasks\n"
        "  todo done <id>                mark task complete\n"
        "  todo undo <id>                mark task pending\n"
        "  todo rm <id>                  delete a task\n"
        "  todo edit <id> [desc] [-p pri] [-d due]  edit a task\n"
        "  todo search <term>            search tasks\n"
        "  todo clear                    remove completed tasks\n"
        "  todo help                     show this help\n"
        "\n"
        "Priority: high (h), med (m), low (l)  — default: med\n"
        "Due date: YYYY-MM-DD or 'none' to clear\n"
        "\n"
        "Environment:\n"
        "  TODO_DB    database file path (default: ~/.todo.db)\n"
        "  NO_COLOR   disable colored output\n"
    );
}
