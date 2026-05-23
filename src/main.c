#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "db.h"
#include "task.h"
#include "color.h"

int g_color = 1;

static const char *db_path(void) {
    const char *env = getenv("TODO_DB");
    if (env) return env;

    static char path[1024];
    const char *home = getenv("HOME");
    if (!home) home = ".";
    snprintf(path, sizeof(path), "%s/.todo.db", home);
    return path;
}

static int cmd_add(const cli_args_t *a) {
    int prio = a->priority >= 0 ? a->priority : PRIO_MED;
    if (db_add(a->desc, prio, a->due) != 0) {
        fprintf(stderr, "error: failed to add task\n");
        return 1;
    }
    printf("Added: %s\n", a->desc);
    return 0;
}

static int cmd_list(const cli_args_t *a) {
    task_t *tasks = NULL;
    int count = 0;
    if (db_list(&tasks, &count, a->filter) != 0) {
        fprintf(stderr, "error: failed to list tasks\n");
        return 1;
    }
    task_print_list(tasks, count);
    free(tasks);
    return 0;
}

static int cmd_done(const cli_args_t *a) {
    if (db_complete(a->id) != 0) {
        fprintf(stderr, "error: task #%d not found or already done\n", a->id);
        return 1;
    }
    printf("Completed task #%d\n", a->id);
    return 0;
}

static int cmd_undo(const cli_args_t *a) {
    if (db_uncomplete(a->id) != 0) {
        fprintf(stderr, "error: task #%d not found or not completed\n", a->id);
        return 1;
    }
    printf("Reopened task #%d\n", a->id);
    return 0;
}

static int cmd_rm(const cli_args_t *a) {
    task_t t;
    if (db_get(a->id, &t) != 0) {
        fprintf(stderr, "error: task #%d not found\n", a->id);
        return 1;
    }
    if (db_delete(a->id) != 0) {
        fprintf(stderr, "error: failed to delete task #%d\n", a->id);
        return 1;
    }
    printf("Deleted: %s\n", t.desc);
    return 0;
}

static int cmd_edit(const cli_args_t *a) {
    if (db_edit(a->id, a->desc, a->priority, a->due) != 0) {
        fprintf(stderr, "error: task #%d not found\n", a->id);
        return 1;
    }
    task_t t;
    if (db_get(a->id, &t) == 0)
        task_print_one(&t);
    return 0;
}

static int cmd_search(const cli_args_t *a) {
    task_t *tasks = NULL;
    int count = 0;
    if (db_search(a->search, &tasks, &count) != 0) {
        fprintf(stderr, "error: search failed\n");
        return 1;
    }
    if (count == 0)
        printf("No tasks matching \"%s\"\n", a->search);
    else
        task_print_list(tasks, count);
    free(tasks);
    return 0;
}

static int cmd_clear(void) {
    int removed = 0;
    if (db_clear_done(&removed) != 0) {
        fprintf(stderr, "error: failed to clear tasks\n");
        return 1;
    }
    if (removed == 0)
        printf("No completed tasks to clear\n");
    else
        printf("Cleared %d completed task%s\n", removed, removed == 1 ? "" : "s");
    return 0;
}

int main(int argc, char **argv) {
    if (getenv("NO_COLOR")) g_color = 0;

    cli_args_t args;
    if (cli_parse(argc, argv, &args) != 0)
        return 1;

    if (strcmp(args.command, "help") == 0) {
        cli_usage();
        return 0;
    }

    if (db_open(db_path()) != 0) {
        fprintf(stderr, "error: could not open database\n");
        return 1;
    }

    int rc = 0;

    if      (strcmp(args.command, "add")    == 0) rc = cmd_add(&args);
    else if (strcmp(args.command, "list")   == 0) rc = cmd_list(&args);
    else if (strcmp(args.command, "done")   == 0) rc = cmd_done(&args);
    else if (strcmp(args.command, "undo")   == 0) rc = cmd_undo(&args);
    else if (strcmp(args.command, "rm")     == 0) rc = cmd_rm(&args);
    else if (strcmp(args.command, "edit")   == 0) rc = cmd_edit(&args);
    else if (strcmp(args.command, "search") == 0) rc = cmd_search(&args);
    else if (strcmp(args.command, "clear")  == 0) rc = cmd_clear();

    db_close();
    return rc;
}
