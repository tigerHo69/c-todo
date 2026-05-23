#ifndef COLOR_H
#define COLOR_H

extern int g_color;

static inline const char *clr(const char *code) {
    return g_color ? code : "";
}

#define CLR_RESET  "\033[0m"
#define CLR_BOLD   "\033[1m"
#define CLR_DIM    "\033[2m"
#define CLR_STRIKE "\033[9m"
#define CLR_RED    "\033[31m"
#define CLR_GREEN  "\033[32m"
#define CLR_YELLOW "\033[33m"
#define CLR_CYAN   "\033[36m"

#endif
