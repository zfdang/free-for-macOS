/*
 * free.h
 * Memory usage utility for macOS (similar to 'free' under Linux)
 *
 * by:  Zhengfa
 *
 */

#define FREE_USAGE "Usage: %s [-h] [-s delay] [-v]\n"

typedef struct mem {
    char total[14];
    char used[14];
    char free[14];
    char app[14];
    char wired[14];
    char cached[14];
} mem_t;
