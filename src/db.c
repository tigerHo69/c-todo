#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "db.h"

static sqlite3 *db;

static const char *SCHEMA =
    "CREATE TABLE IF NOT EXISTS tasks ("
    "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "  description TEXT NOT NULL,"
    "  priority INTEGER DEFAULT 1,"
    "  due_date TEXT DEFAULT '',"
    "  created_at TEXT DEFAULT (datetime('now','localtime')),"
    "  completed INTEGER DEFAULT 0,"
    "  completed_at TEXT DEFAULT ''"
    ")";

int db_open(const char *path) {
    if (sqlite3_open(path, &db) != SQLITE_OK) {
        fprintf(stderr, "db: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    char *err = NULL;
    if (sqlite3_exec(db, SCHEMA, NULL, NULL, &err) != SQLITE_OK) {
        fprintf(stderr, "db: %s\n", err);
        sqlite3_free(err);
        return -1;
    }
    return 0;
}

void db_close(void) {
    if (db) sqlite3_close(db);
    db = NULL;
}

static void row_to_task(sqlite3_stmt *stmt, task_t *t) {
    t->id       = sqlite3_column_int(stmt, 0);
    const char *d = (const char *)sqlite3_column_text(stmt, 1);
    snprintf(t->desc, sizeof(t->desc), "%s", d ? d : "");
    t->priority = sqlite3_column_int(stmt, 2);
    const char *due = (const char *)sqlite3_column_text(stmt, 3);
    snprintf(t->due, sizeof(t->due), "%s", due ? due : "");
    const char *cr = (const char *)sqlite3_column_text(stmt, 4);
    snprintf(t->created, sizeof(t->created), "%s", cr ? cr : "");
    t->done     = sqlite3_column_int(stmt, 5);
    const char *da = (const char *)sqlite3_column_text(stmt, 6);
    snprintf(t->done_at, sizeof(t->done_at), "%s", da ? da : "");
}

int db_add(const char *desc, int priority, const char *due) {
    const char *sql =
        "INSERT INTO tasks (description, priority, due_date) VALUES (?, ?, ?)";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;
    sqlite3_bind_text(stmt, 1, desc, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, priority);
    sqlite3_bind_text(stmt, 3, due ? due : "", -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt) == SQLITE_DONE ? 0 : -1;
    sqlite3_finalize(stmt);
    return rc;
}

static int fetch_tasks(const char *sql, task_t **out, int *count,
                       const char *bind_text) {
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;
    if (bind_text)
        sqlite3_bind_text(stmt, 1, bind_text, -1, SQLITE_STATIC);

    int cap = 16;
    *count = 0;
    *out = malloc((size_t)cap * sizeof(task_t));
    if (!*out) { sqlite3_finalize(stmt); return -1; }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        if (*count >= cap) {
            cap *= 2;
            task_t *tmp = realloc(*out, (size_t)cap * sizeof(task_t));
            if (!tmp) { free(*out); *out = NULL; sqlite3_finalize(stmt); return -1; }
            *out = tmp;
        }
        row_to_task(stmt, &(*out)[*count]);
        (*count)++;
    }
    sqlite3_finalize(stmt);
    return 0;
}

static const char *COLS =
    "id, description, priority, due_date, created_at, completed, completed_at";

int db_list(task_t **out, int *count, int filter) {
    char sql[512];
    switch (filter) {
    case FILTER_DONE:
        snprintf(sql, sizeof(sql),
            "SELECT %s FROM tasks WHERE completed = 1 "
            "ORDER BY completed_at DESC", COLS);
        break;
    case FILTER_ALL:
        snprintf(sql, sizeof(sql),
            "SELECT %s FROM tasks "
            "ORDER BY completed ASC, priority DESC, "
            "CASE WHEN due_date = '' THEN 1 ELSE 0 END, due_date ASC, id ASC",
            COLS);
        break;
    default:
        snprintf(sql, sizeof(sql),
            "SELECT %s FROM tasks WHERE completed = 0 "
            "ORDER BY priority DESC, "
            "CASE WHEN due_date = '' THEN 1 ELSE 0 END, due_date ASC, id ASC",
            COLS);
        break;
    }
    return fetch_tasks(sql, out, count, NULL);
}

int db_get(int id, task_t *out) {
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT %s FROM tasks WHERE id = ?", COLS);
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;
    sqlite3_bind_int(stmt, 1, id);
    int rc = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        row_to_task(stmt, out);
        rc = 0;
    }
    sqlite3_finalize(stmt);
    return rc;
}

static int update_completed(int id, int value) {
    const char *sql = value
        ? "UPDATE tasks SET completed = 1, completed_at = datetime('now','localtime') WHERE id = ? AND completed = 0"
        : "UPDATE tasks SET completed = 0, completed_at = '' WHERE id = ? AND completed = 1";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;
    sqlite3_bind_int(stmt, 1, id);
    sqlite3_step(stmt);
    int changed = sqlite3_changes(db);
    sqlite3_finalize(stmt);
    return changed > 0 ? 0 : -1;
}

int db_complete(int id)   { return update_completed(id, 1); }
int db_uncomplete(int id) { return update_completed(id, 0); }

int db_delete(int id) {
    const char *sql = "DELETE FROM tasks WHERE id = ?";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;
    sqlite3_bind_int(stmt, 1, id);
    sqlite3_step(stmt);
    int changed = sqlite3_changes(db);
    sqlite3_finalize(stmt);
    return changed > 0 ? 0 : -1;
}

int db_edit(int id, const char *desc, int priority, const char *due) {
    task_t current;
    if (db_get(id, &current) != 0) return -1;

    const char *new_desc = desc ? desc : current.desc;
    int new_prio = priority >= 0 ? priority : current.priority;
    const char *new_due;
    if (!due)
        new_due = current.due;
    else if (strcmp(due, "none") == 0)
        new_due = "";
    else
        new_due = due;

    const char *sql =
        "UPDATE tasks SET description = ?, priority = ?, due_date = ? WHERE id = ?";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;
    sqlite3_bind_text(stmt, 1, new_desc, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, new_prio);
    sqlite3_bind_text(stmt, 3, new_due, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, id);
    int rc = sqlite3_step(stmt) == SQLITE_DONE ? 0 : -1;
    sqlite3_finalize(stmt);
    return rc;
}

int db_search(const char *term, task_t **out, int *count) {
    char sql[512];
    snprintf(sql, sizeof(sql),
        "SELECT %s FROM tasks WHERE description LIKE ? "
        "ORDER BY completed ASC, priority DESC, id ASC", COLS);
    char pattern[270];
    snprintf(pattern, sizeof(pattern), "%%%s%%", term);
    return fetch_tasks(sql, out, count, pattern);
}

int db_clear_done(int *removed) {
    const char *sql = "DELETE FROM tasks WHERE completed = 1";
    char *err = NULL;
    if (sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK) {
        fprintf(stderr, "db: %s\n", err);
        sqlite3_free(err);
        return -1;
    }
    *removed = sqlite3_changes(db);
    return 0;
}
