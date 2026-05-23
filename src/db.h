#ifndef DB_H
#define DB_H

#include "task.h"

enum {
    FILTER_PENDING = 0,
    FILTER_DONE    = 1,
    FILTER_ALL     = 2,
};

int  db_open(const char *path);
void db_close(void);
int  db_add(const char *desc, int priority, const char *due);
int  db_list(task_t **out, int *count, int filter);
int  db_get(int id, task_t *out);
int  db_complete(int id);
int  db_uncomplete(int id);
int  db_delete(int id);
int  db_edit(int id, const char *desc, int priority, const char *due);
int  db_search(const char *term, task_t **out, int *count);
int  db_clear_done(int *removed);

#endif
