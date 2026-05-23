#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../src/db.h"
#include "../src/task.h"

int g_color = 0;

static const char *DB = "/tmp/test_todo_unit.db";

static void setup(void) {
    unlink(DB);
    assert(db_open(DB) == 0);
}

static void teardown(void) {
    db_close();
    unlink(DB);
}

static void test_add_and_list(void) {
    setup();
    assert(db_add("Task one", PRIO_HIGH, "2026-06-01") == 0);
    assert(db_add("Task two", PRIO_MED, "") == 0);
    assert(db_add("Task three", PRIO_LOW, "2026-12-25") == 0);

    task_t *tasks = NULL;
    int count = 0;
    assert(db_list(&tasks, &count, FILTER_ALL) == 0);
    assert(count == 3);
    assert(tasks[0].priority == PRIO_HIGH);
    assert(strcmp(tasks[0].desc, "Task one") == 0);
    free(tasks);
    teardown();
    printf("  PASS: add_and_list\n");
}

static void test_complete_and_undo(void) {
    setup();
    db_add("Complete me", PRIO_MED, "");

    task_t *tasks = NULL;
    int count = 0;
    db_list(&tasks, &count, FILTER_PENDING);
    assert(count == 1);
    int id = tasks[0].id;
    free(tasks);

    assert(db_complete(id) == 0);
    assert(db_complete(id) == -1);

    db_list(&tasks, &count, FILTER_PENDING);
    assert(count == 0);
    free(tasks);

    db_list(&tasks, &count, FILTER_DONE);
    assert(count == 1);
    assert(tasks[0].done == 1);
    free(tasks);

    assert(db_uncomplete(id) == 0);
    assert(db_uncomplete(id) == -1);

    db_list(&tasks, &count, FILTER_PENDING);
    assert(count == 1);
    assert(tasks[0].done == 0);
    free(tasks);
    teardown();
    printf("  PASS: complete_and_undo\n");
}

static void test_delete(void) {
    setup();
    db_add("Delete me", PRIO_LOW, "");

    task_t *tasks = NULL;
    int count = 0;
    db_list(&tasks, &count, FILTER_ALL);
    int id = tasks[0].id;
    free(tasks);

    assert(db_delete(id) == 0);
    assert(db_delete(id) == -1);

    db_list(&tasks, &count, FILTER_ALL);
    assert(count == 0);
    free(tasks);
    teardown();
    printf("  PASS: delete\n");
}

static void test_edit(void) {
    setup();
    db_add("Original", PRIO_LOW, "2026-01-01");

    task_t *tasks = NULL;
    int count = 0;
    db_list(&tasks, &count, FILTER_ALL);
    int id = tasks[0].id;
    free(tasks);

    assert(db_edit(id, "Updated", PRIO_HIGH, "2026-06-15") == 0);

    task_t t;
    assert(db_get(id, &t) == 0);
    assert(strcmp(t.desc, "Updated") == 0);
    assert(t.priority == PRIO_HIGH);
    assert(strcmp(t.due, "2026-06-15") == 0);

    assert(db_edit(id, NULL, -1, "none") == 0);
    assert(db_get(id, &t) == 0);
    assert(strcmp(t.desc, "Updated") == 0);
    assert(t.due[0] == '\0');

    assert(db_edit(999, "Nope", -1, NULL) == -1);
    teardown();
    printf("  PASS: edit\n");
}

static void test_search(void) {
    setup();
    db_add("Buy groceries", PRIO_MED, "");
    db_add("Buy a car", PRIO_HIGH, "");
    db_add("Read a book", PRIO_LOW, "");

    task_t *tasks = NULL;
    int count = 0;
    assert(db_search("Buy", &tasks, &count) == 0);
    assert(count == 2);
    free(tasks);

    assert(db_search("zzz_no_match", &tasks, &count) == 0);
    assert(count == 0);
    free(tasks);
    teardown();
    printf("  PASS: search\n");
}

static void test_clear_done(void) {
    setup();
    db_add("Keep", PRIO_MED, "");
    db_add("Clear me", PRIO_LOW, "");

    task_t *tasks = NULL;
    int count = 0;
    db_list(&tasks, &count, FILTER_ALL);
    db_complete(tasks[1].id);
    free(tasks);

    int removed = 0;
    assert(db_clear_done(&removed) == 0);
    assert(removed == 1);

    db_list(&tasks, &count, FILTER_ALL);
    assert(count == 1);
    assert(strcmp(tasks[0].desc, "Keep") == 0);
    free(tasks);
    teardown();
    printf("  PASS: clear_done\n");
}

static void test_sort_order(void) {
    setup();
    db_add("Low no date", PRIO_LOW, "");
    db_add("High late", PRIO_HIGH, "2026-12-01");
    db_add("High early", PRIO_HIGH, "2026-06-01");
    db_add("Med", PRIO_MED, "2026-09-01");

    task_t *tasks = NULL;
    int count = 0;
    db_list(&tasks, &count, FILTER_PENDING);
    assert(count == 4);
    assert(strcmp(tasks[0].desc, "High early") == 0);
    assert(strcmp(tasks[1].desc, "High late") == 0);
    assert(strcmp(tasks[2].desc, "Med") == 0);
    assert(strcmp(tasks[3].desc, "Low no date") == 0);
    free(tasks);
    teardown();
    printf("  PASS: sort_order\n");
}

int main(void) {
    printf("test_db:\n");
    test_add_and_list();
    test_complete_and_undo();
    test_delete();
    test_edit();
    test_search();
    test_clear_done();
    test_sort_order();
    printf("All db tests passed.\n");
    return 0;
}
