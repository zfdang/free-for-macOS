/*
 * File: free.c
 * Author: Zhengfa
 * Description: This program displays memory usage statistics on macOS.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>
#include <mach/mach.h>
#include <sys/sysctl.h>

#include "free.h"

extern char *optarg;
extern int optind;


void formatBytes(unsigned long long bytes, char *buffer, int bufferSize, int human) {
    if(human == 0) {
        snprintf(buffer, bufferSize, "%llu", bytes);
        return;
    }
    
    const char *suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    int suffixIndex = 0;
    double result = 0.0;

    while (bytes > 1024 && suffixIndex < sizeof(suffixes) / sizeof(suffixes[0])) {
        result = bytes / 1024.0;
        bytes /= 1024;
        suffixIndex++;
    }

    snprintf(buffer, bufferSize, "%.2f %s", result, suffixes[suffixIndex]);
}

// void dumpVmStat(vm_statistics64_data_t *vm, long pagesize, long TotalRAM) {
//     char value[32];

//     // raw counts
//     formatBytes(vm->active_count * pagesize, value, sizeof(value), 1);
//     printf("active_count memory %s\n", value);

//     formatBytes(vm->inactive_count * pagesize, value, sizeof(value), 1);
//     printf("inactive_count memory %s\n", value);
    
//     formatBytes(vm->wire_count * pagesize, value, sizeof(value), 1);
//     printf("wire_count memory %s\n", value);

//     formatBytes(vm->free_count * pagesize, value, sizeof(value), 1);
//     printf("free_count memory %s\n", value);

//     formatBytes(vm->internal_page_count * pagesize, value, sizeof(value), 1);
//     printf("internal_page_count memory %s\n", value);
    
//     formatBytes(vm->speculative_count * pagesize, value, sizeof(value), 1);
//     printf("speculative_count memory %s\n", value);
    
//     formatBytes(vm->compressor_page_count * pagesize, value, sizeof(value), 1);
//     printf("compressor_page_count memory %s\n", value);

//     formatBytes(vm->purgeable_count * pagesize, value, sizeof(value), 1);
//     printf("purgeable_count memory %s\n", value);

//     formatBytes(vm->external_page_count * pagesize, value, sizeof(value), 1);
//     printf("external_page_count memory %s\n", value);

//     // derived numbers
//     formatBytes((vm->purgeable_count + vm->external_page_count )* pagesize, value, sizeof(value), 1);
//     printf("cached = purgeable_count + external_page_count %s\n", value);

//     formatBytes((vm->internal_page_count - vm->purgeable_count )* pagesize, value, sizeof(value), 1);
//     printf("app = internal_page_count - purgeable_count %s\n", value);

//     formatBytes((vm->free_count - vm->speculative_count )* pagesize, value, sizeof(value), 1);
//     printf("truely free = free_count - speculative_count %s\n", value);

//     formatBytes(TotalRAM - (vm->free_count - vm->speculative_count + vm->purgeable_count + vm->external_page_count)* pagesize, value, sizeof(value), 1);
//     printf("used = total - truely-free - cached) %s\n", value);
// }


/* print usage information */
int main(int argc, char **argv) {
    // show memory usage periodically
    int poll = 0;
    // show results in human readable format
    int human = 0;
    char c;    
    kern_return_t ke = KERN_SUCCESS;
    mach_port_t host;
    struct host_basic_info hbi;
    vm_size_t page_size = 4096;
    vm_statistics64_data_t vm_stat;
    mach_msg_type_number_t hbi_sz, vm_stat_sz;
    mem_t mem;
    mem_t swap;

    struct xsw_usage swapinfo; /* defined in sysctl.h */
    size_t swapinfo_sz = sizeof(swapinfo);
    int vmmib[2] = {CTL_VM, VM_SWAPUSAGE};


    /* parse our command line options */
    while ((c = getopt(argc, argv, "hs:v")) != -1) {
        if (c == 'h') {
            human = 1;
        } else if (c == 's') {
            poll = atoi(optarg);
        } else if (c == 'v') {
            printf("free: version %s\n", _FREE_VERSION);
            return EXIT_SUCCESS;
        } else {
            printf(FREE_USAGE, basename(argv[0]));
            return EXIT_FAILURE;
        }
    }

    host = mach_host_self();

    /* set some preferred sizes */
    hbi_sz = sizeof(hbi) / sizeof(integer_t);
    // vm_stat_sz = sizeof(vm_stat) / sizeof(integer_t);
    vm_stat_sz = HOST_VM_INFO64_COUNT;

    /* get basic system information */
    ke = host_info(host, HOST_BASIC_INFO, (host_info_t) &hbi, &hbi_sz);
    if (ke != KERN_SUCCESS) {
        mach_error("host_info", ke);
        return EXIT_FAILURE;
    }

    /* get the virtual memory page size for this machine */
    if ((ke = host_page_size(host, &page_size)) != KERN_SUCCESS) {
        mach_error("host_page_size", ke);
        return EXIT_FAILURE;
    }

    /* loop over this in case we are polling */
    while (1) {
        /* gather virtual memory statistics */
        ke = host_statistics64(host, HOST_VM_INFO64, (host_info64_t) &vm_stat, &vm_stat_sz);
        if (ke != KERN_SUCCESS) {
            mach_error("host_statistics", ke);
            return EXIT_FAILURE;
        }

        // method to dump metrics for calculation verification
        // dumpVmStat(&vm_stat, page_size, hbi.max_mem);

        /* we have collected data, put it into our structure */
        // total; verified
        formatBytes(hbi.max_mem, mem.total, sizeof(mem.total), human);
        // free = free_count - speculative_count; verified
        formatBytes((vm_stat.free_count - vm_stat.speculative_count) * page_size, mem.free, sizeof(mem.free), human);
        // wired; verified
        formatBytes(vm_stat.wire_count * page_size, mem.wired, sizeof(mem.wired), human);
        // cached file = purgeable_count + external_page_count; verified
        formatBytes((vm_stat.purgeable_count + vm_stat.external_page_count) * page_size, mem.cached, sizeof(mem.cached), human);
        // truely-free = free_count - speculative_count
        // used = total - truely-free - cached; partially verified, still small discrepancy
        formatBytes(hbi.max_mem - (vm_stat.free_count - vm_stat.speculative_count + vm_stat.purgeable_count + vm_stat.external_page_count) * page_size, mem.used, sizeof(mem.used), human);
        // app memory = internal_page_count - purgeable_count; verified
        formatBytes((vm_stat.internal_page_count - vm_stat.purgeable_count) * page_size, mem.app, sizeof(mem.app), human);
       
        // get swap info
        if (sysctl(vmmib, 2, &swapinfo, &swapinfo_sz, NULL, 0) == -1) {
            fprintf(stderr, "Could not collect VM info, errno %d - %s", errno, strerror(errno));
            return EXIT_FAILURE;
        }

        formatBytes(swapinfo.xsu_total, swap.total, sizeof(swap.total), human);
        formatBytes(swapinfo.xsu_used, swap.used, sizeof(swap.used), human);
        formatBytes(swapinfo.xsu_avail, swap.free, sizeof(swap.free), human);

        /* print the header */
        printf("%20s %14s %14s %14s %14s %14s\n",
            "total", "used", "free", "cached", "app", "wired");
        /* display the memory usage statistics */
         printf("Mem: %15s %14s %14s %14s %14s %14s\n",
             mem.total, mem.used, mem.free, mem.cached, mem.app, mem.wired);
         printf("Swap: %14s %14s %14s\n", swap.total, swap.used, swap.free);

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
