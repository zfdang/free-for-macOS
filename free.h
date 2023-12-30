/*
 * free.h
 * Memory usage utility for macOS (similar to 'free' under Linux)
 *
 * by:  Zhengfa
 *
 */

#define FREE_USAGE "Usage: %s [-b|-k|-m] [-s delay] [-V] [-h|-?]\n"
#define COMBINED_UNIT_OPTIONS "You may only use one of the unit options:  -b, -k, or -m\n"

enum { UNUSED_UNIT, BYTES, KILOBYTES, MEGABYTES };

typedef struct mem {
    uint64_t total;
    uint64_t used;
    uint64_t free;
    uint64_t active;
    uint64_t inactive;
    uint64_t wired;
} mem_t;
