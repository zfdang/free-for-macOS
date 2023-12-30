/*
 * free.c
 * Memory usage utility for Darwin & MacOS X (similar to 'free' under Linux)
 *
 * by:  David Cantrell <david.l.cantrell@gmail.com>
 * Copyright (C) 2002, David Cantrell, Atlanta, GA, USA.
 * Copyright (C) 2008, David Cantrell, Honolulu, HI, USA.
 *
 * Licensed under the GNU Lesser General Public License version 2.1 or later.
 * See COPYING.LIB for licensing details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include <mach/mach.h>
#include <mach/mach_error.h>
#include <mach/kern_return.h>
#include <mach/host_info.h>
#include <mach/host_priv.h>

#include "free.h"

extern char *optarg;
extern int optind;

static void set_units(int *units, int type) {
    if (*units != -1) {
        fprintf(stderr, COMBINED_UNIT_OPTIONS);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    *units = type;
}

int main(int argc, char **argv) {
    int poll = 0, div = 1, units = -1;
    char c;
    kern_return_t ke = KERN_SUCCESS;
    mach_port_t host, task;
    vm_size_t hps;
    vm_statistics_data_t hs;
    struct host_basic_info hbi;
    mem_t ms;
    mach_msg_type_number_t memsz, vmsz;

    /* parse our command line options */
    while ((c = getopt(argc, argv, "bkms:Vh?")) != -1) {
        if (c == 'b') {
            set_units(&units, BYTES);
        } else if (c == 'k') {
            set_units(&units, KILOBYTES);
        } else if (c == 'm') {
            set_units(&units, MEGABYTES);
        } else if (c == 's') {
            poll = atoi(optarg);
        } else if (c == 'V') {
            printf("darwin-free version %s\n", _FREE_VERSION);
            return EXIT_SUCCESS;
        } else {
            printf(FREE_USAGE, basename(argv[0]));

            if (c == 'h' || c == '?') {
                return EXIT_SUCCESS;
            }

            return EXIT_FAILURE;
        }
    }

    if (units == -1) {
        units = KILOBYTES;
    }

    host = mach_host_self();
    task = mach_task_self();

    /* set some preferred sizes */
    memsz = sizeof(hbi) / sizeof(integer_t);
    vmsz = sizeof(hs) / sizeof(integer_t);

    /* get basic system information */
    ke = host_info(host, HOST_BASIC_INFO, (host_info_t) &hbi, &memsz);
    if (ke != KERN_SUCCESS) {
        mach_error("host_info", ke);
        return EXIT_FAILURE;
    }

    /* loop over this in case we are polling */
    while (1) {
        /* get the virtual memory page size for this machine */
        if ((ke = host_page_size(host, &hps)) != KERN_SUCCESS) {
            mach_error("host_page_size", ke);
            return EXIT_FAILURE;
        }

        /* gather virtual memory statistics */
        ke = host_statistics(host, HOST_VM_INFO, (host_info_t) &hs, &vmsz);
        if (ke != KERN_SUCCESS) {
            mach_error("host_statistics", ke);
            return EXIT_FAILURE;
        }

        /* select divisor */
        if (units == KILOBYTES) {
            div = 1024;
        } else if (units == MEGABYTES) {
            div = 1048576;
        }

        /* we have collected data, put it into our structure */
        ms.total = hbi.max_mem / div;
        ms.used = ((hs.active_count + hs.inactive_count + hs.wire_count)
                   * hps) / div;
        ms.free = (hs.free_count * hps) / div;
        ms.active = (hs.active_count * hps) / div;
        ms.inactive = (hs.inactive_count * hps) / div;
        ms.wired = (hs.wire_count * hps) / div;

        /* display the memory usage statistics */
        printf("%18s %10s %10s %10s %10s %10s\n",
               "total", "used", "free", "active", "inactive", "wired");
        printf("Mem: %13llu %10llu %10llu %10llu %10llu %10llu\n",
               ms.total, ms.used, ms.free,
               ms.active, ms.inactive, ms.wired);

        /* does the loop continue? */
        if (poll != 0) {
            sleep(poll);
            printf("\n");
        } else {
            break;
        }
    }

    return EXIT_SUCCESS;
}
