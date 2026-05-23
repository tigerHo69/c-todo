#ifndef TASK_H
#define TASK_H

enum {
    PRIO_LOW  = 0,
    PRIO_MED  = 1,
    PRIO_HIGH = 2,
};

typedef struct {
    int  id;
    char desc[256];
    int  priority;
    char due[11];
    char created[20];
    int  done;
    char done_at[20];
} task_t;

int         prio_from_str(const char *s);
const char *prio_to_str(int p);
void        task_print_list(const task_t *tasks, int count);
void        task_print_one(const task_t *t);

#endif
