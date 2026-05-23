#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/cli.h"
#include "../src/task.h"
#include "../src/db.h"

int g_color = 0;

static void test_no_args_defaults_to_list(void) {
    char *argv[] = {"todo"};
    cli_args_t args;
    assert(cli_parse(1, argv, &args) == 0);
    assert(strcmp(args.command, "list") == 0);
    printf("  PASS: no_args_defaults_to_list\n");
}

static void test_add_with_flags(void) {
    char *argv[] = {"todo", "add", "Buy milk", "-p", "high", "-d", "2026-06-01"};
    cli_args_t args;
    assert(cli_parse(7, argv, &args) == 0);
    assert(strcmp(args.command, "add") == 0);
    assert(strcmp(args.desc, "Buy milk") == 0);
    assert(args.priority == PRIO_HIGH);
    assert(strcmp(args.due, "2026-06-01") == 0);
    printf("  PASS: add_with_flags\n");
}

static void test_add_minimal(void) {
    char *argv[] = {"todo", "add", "Simple task"};
    cli_args_t args;
    assert(cli_parse(3, argv, &args) == 0);
    assert(strcmp(args.desc, "Simple task") == 0);
    assert(args.priority == -1);
    assert(args.due == NULL);
    printf("  PASS: add_minimal\n");
}

static void test_done_with_id(void) {
    char *argv[] = {"todo", "done", "42"};
    cli_args_t args;
    assert(cli_parse(3, argv, &args) == 0);
    assert(strcmp(args.command, "done") == 0);
    assert(args.id == 42);
    printf("  PASS: done_with_id\n");
}

static void test_list_filters(void) {
    char *all[]     = {"todo", "list", "--all"};
    char *done[]    = {"todo", "list", "--done"};
    char *pending[] = {"todo", "list", "--pending"};
    cli_args_t args;

    assert(cli_parse(3, all, &args) == 0);
    assert(args.filter == FILTER_ALL);

    assert(cli_parse(3, done, &args) == 0);
    assert(args.filter == FILTER_DONE);

    assert(cli_parse(3, pending, &args) == 0);
    assert(args.filter == FILTER_PENDING);
    printf("  PASS: list_filters\n");
}

static void test_edit_partial(void) {
    char *argv[] = {"todo", "edit", "5", "-p", "low"};
    cli_args_t args;
    assert(cli_parse(5, argv, &args) == 0);
    assert(strcmp(args.command, "edit") == 0);
    assert(args.id == 5);
    assert(args.priority == PRIO_LOW);
    assert(args.desc == NULL);
    printf("  PASS: edit_partial\n");
}

static void test_search(void) {
    char *argv[] = {"todo", "search", "groceries"};
    cli_args_t args;
    assert(cli_parse(3, argv, &args) == 0);
    assert(strcmp(args.command, "search") == 0);
    assert(strcmp(args.search, "groceries") == 0);
    printf("  PASS: search\n");
}

static void test_invalid_command(void) {
    char *argv[] = {"todo", "bogus"};
    cli_args_t args;
    FILE *devnull = freopen("/dev/null", "w", stderr);
    assert(cli_parse(2, argv, &args) != 0);
    freopen("/dev/tty", "w", stderr);
    (void)devnull;
    printf("  PASS: invalid_command\n");
}

static void test_missing_add_desc(void) {
    char *argv[] = {"todo", "add"};
    cli_args_t args;
    FILE *devnull = freopen("/dev/null", "w", stderr);
    assert(cli_parse(2, argv, &args) != 0);
    freopen("/dev/tty", "w", stderr);
    (void)devnull;
    printf("  PASS: missing_add_desc\n");
}

static void test_priority_shorthand(void) {
    char *argv[] = {"todo", "add", "test", "-p", "h"};
    cli_args_t args;
    assert(cli_parse(5, argv, &args) == 0);
    assert(args.priority == PRIO_HIGH);
    printf("  PASS: priority_shorthand\n");
}

int main(void) {
    printf("test_cli:\n");
    test_no_args_defaults_to_list();
    test_add_with_flags();
    test_add_minimal();
    test_done_with_id();
    test_list_filters();
    test_edit_partial();
    test_search();
    test_invalid_command();
    test_missing_add_desc();
    test_priority_shorthand();
    printf("All cli tests passed.\n");
    return 0;
}
