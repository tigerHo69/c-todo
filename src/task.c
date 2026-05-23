#include <stdio.h>
#include <string.h>
#include <time.h>
#include "task.h"
#include "color.h"

int prio_from_str(const char *s) {
    if (!s) return -1;
    if (strcmp(s, "high") == 0 || strcmp(s, "h") == 0) return PRIO_HIGH;
    if (strcmp(s, "med")  == 0 || strcmp(s, "m") == 0) return PRIO_MED;
    if (strcmp(s, "low")  == 0 || strcmp(s, "l") == 0) return PRIO_LOW;
    return -1;
}

const char *prio_to_str(int p) {
    switch (p) {
        case PRIO_HIGH: return "high";
        case PRIO_MED:  return "med";
        case PRIO_LOW:  return "low";
        default:        return "med";
    }
}

static const char *prio_color(int p) {
    switch (p) {
        case PRIO_HIGH: return CLR_RED;
        case PRIO_MED:  return CLR_YELLOW;
        default:        return CLR_DIM;
    }
}

static const char *format_date(const char *iso) {
    static char buf[16];
    if (!iso || !*iso) return "";

    int y, m, d;
    if (sscanf(iso, "%d-%d-%d", &y, &m, &d) != 3) return iso;

    static const char *months[] = {
        "Jan","Feb","Mar","Apr","May","Jun",
        "Jul","Aug","Sep","Oct","Nov","Dec"
    };
    if (m < 1 || m > 12) return iso;

    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    int cur_year = tm->tm_year + 1900;

    if (y != cur_year)
        snprintf(buf, sizeof(buf), "%s %02d, %d", months[m-1], d, y);
    else
        snprintf(buf, sizeof(buf), "%s %02d", months[m-1], d);

    return buf;
}

static int is_overdue(const char *due, int done) {
    if (done || !due || !*due) return 0;

    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char today[11];
    strftime(today, sizeof(today), "%Y-%m-%d", tm);

    return strcmp(due, today) < 0;
}

static void print_task_row(const task_t *t, int desc_width) {
    char desc_buf[54];
    if ((int)strlen(t->desc) > desc_width) {
        snprintf(desc_buf, sizeof(desc_buf), "%.*s...", desc_width - 3, t->desc);
    } else {
        snprintf(desc_buf, sizeof(desc_buf), "%s", t->desc);
    }

    const char *dim_start = t->done ? clr(CLR_DIM) : "";
    const char *dim_end   = t->done ? clr(CLR_RESET) : "";
    const char *check     = t->done ? "✓ " : "  ";

    const char *pc = prio_color(t->priority);
    const char *date_str = format_date(t->due);
    int overdue = is_overdue(t->due, t->done);

    printf("  %s%4d  %s%-*s  %s%-4s%s",
        dim_start, t->id, check, desc_width, desc_buf,
        clr(pc), prio_to_str(t->priority), clr(CLR_RESET));

    if (*date_str) {
        if (overdue)
            printf("  %s%s%s", clr(CLR_RED), date_str, clr(CLR_RESET));
        else
            printf("  %s%s%s", dim_start, date_str, dim_end);
    }

    printf("%s\n", dim_end);
}

void task_print_list(const task_t *tasks, int count) {
    if (count == 0) {
        printf("No tasks. Add one with: todo add \"description\"\n");
        return;
    }

    int max_desc = 11;
    for (int i = 0; i < count; i++) {
        int len = (int)strlen(tasks[i].desc) + (tasks[i].done ? 2 : 2);
        if (len > max_desc) max_desc = len;
    }
    if (max_desc > 50) max_desc = 50;

    printf("  %s%4s  %-*s  %-4s  %s%s\n",
        clr(CLR_BOLD), "ID", max_desc, "Description",
        "Pri", "Due", clr(CLR_RESET));

    for (int i = 0; i < count; i++)
        print_task_row(&tasks[i], max_desc);

    int pending = 0, done = 0;
    for (int i = 0; i < count; i++) {
        if (tasks[i].done) done++; else pending++;
    }

    printf("\n");
    if (done > 0 && pending > 0)
        printf("%d pending, %d done (%d total)\n", pending, done, count);
    else if (done > 0)
        printf("%d completed task%s\n", done, done == 1 ? "" : "s");
    else
        printf("%d pending task%s\n", pending, pending == 1 ? "" : "s");
}

void task_print_one(const task_t *t) {
    printf("  ID:       %d\n", t->id);
    printf("  Desc:     %s\n", t->desc);
    printf("  Priority: %s\n", prio_to_str(t->priority));
    if (*t->due)
        printf("  Due:      %s\n", format_date(t->due));
    printf("  Created:  %s\n", t->created);
    if (t->done)
        printf("  Done:     %s\n", t->done_at);
    else
        printf("  Status:   pending\n");
}
