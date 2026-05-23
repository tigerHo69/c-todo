#ifndef CLI_H
#define CLI_H

typedef struct {
    const char *command;
    const char *desc;
    int         priority;
    const char *due;
    int         id;
    int         filter;
    const char *search;
} cli_args_t;

int  cli_parse(int argc, char **argv, cli_args_t *args);
void cli_usage(void);

#endif
