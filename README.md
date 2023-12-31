# free for macOS

A memory usage utility for macOS  (similar with Linux Free Command)

This utility is designed to act like 'free' on Linux systems.  It is a tool for displaying memory usage statistics. Since the Mach microkernel design is different than Linux, the values reported are not an exact duplicate of what you can get from a Linux system.  Be sure to read the man page for an explanation of the program output.

```
❯ free -h
               total           used           free         cached            app          wired
Mem:        32.00 GB       18.69 GB      337.48 MB       12.98 GB       15.48 GB        2.04 GB
Swap:         0.00 B         0.00 B         0.00 B
```

# Installation

### HOMEBREW:

   A Homebrew formula is available at https://github.com/zfdang/homebrew-free-for-macOS
   
   To install via Homebrew, run the following:

      brew tap zfdang/free-for-macOS
      brew install free-for-macOS

### COMPILATION:

   You need a compiler, some headers, and a Mach-based system.  On Darwin, you can type make:

      make

   And out will pop a 'free' executable.  On other systems, it may require modification of the Makefile and/or source.

   Either run 

      make install

   or put the free executable in a public place and the man page in a sensible place.

# USAGE:

```
❯ free -v
free: version 1.1.0
❯ free
               total           used           free         cached            app          wired
Mem:     34359738368    20060241920      282902528    14016593920    16661561344     2149351424
Swap:              0              0              0
❯ free -h
               total           used           free         cached            app          wired
Mem:        32.00 GB       18.69 GB      337.48 MB       12.98 GB       15.48 GB        2.04 GB
Swap:         0.00 B         0.00 B         0.00 B
❯ free -h -s 1
               total           used           free         cached            app          wired
Mem:        32.00 GB       18.67 GB      344.30 MB       12.99 GB       15.48 GB        2.03 GB
Swap:         0.00 B         0.00 B         0.00 B

               total           used           free         cached            app          wired
Mem:        32.00 GB       18.66 GB      336.53 MB       13.02 GB       15.42 GB        2.07 GB
Swap:         0.00 B         0.00 B         0.00 B

               total           used           free         cached            app          wired
Mem:        32.00 GB       18.68 GB      337.17 MB       12.99 GB       15.48 GB        2.04 GB
Swap:         0.00 B         0.00 B         0.00 B

               total           used           free         cached            app          wired
Mem:        32.00 GB       18.65 GB      343.44 MB       13.02 GB       15.42 GB        2.06 GB
Swap:         0.00 B         0.00 B         0.00 B
^C
```

or 

	man free

# Explanation:

Total memory is fetched from host_info.

`https://developer.apple.com/documentation/kernel/1502514-host_info`

All other memory information is fetched from host_statistics64.

`https://developer.apple.com/documentation/kernel/1502863-host_statistics64`

Here are the methods to calculate memory statistics:

## free memory

```
// free = free_count - speculative_count; verified
formatBytes((vm_stat.free_count - vm_stat.speculative_count) * page_size, mem.free, sizeof(mem.free), human);

```

## used memory
```
// truely-free = free-count - speculative_count
// used = total - truely-free - cached; partially verified, still has small discrepancy with "activity monitor"
formatBytes(hbi.max_mem - (vm_stat.free_count - vm_stat.speculative_count + vm_stat.purgeable_count + vm_stat.external_page_count) * page_size, mem.used, sizeof(mem.used), human);
```

## cached memory

```
// cached file memory  = (purgeable_count + external_page_count) * page_size
formatBytes((vm_stat.purgeable_count + vm_stat.external_page_count) * page_size, mem.cached, sizeof(mem.cached), human);

```

## app memory

```
// app memory = internal_page_count - purgeable_count; verified
formatBytes((vm_stat.internal_page_count - vm_stat.purgeable_count) * page_size, mem.app, sizeof(mem.app), human);

```

## wired memory

```
Memory that is used by the kernel to run essential system processes. 
It is essentially occupied by the kernel and cannot be freed.
```


# Reference

## mdoc online editor

```
https://roperzh.github.io/grapse/
```

## how to calculate memory in macOS:

```
https://novov.me/blog/posts/mixedmemories
```

```
https://stackoverflow.com/questions/14789672/why-does-host-statistics64-return-inconsistent-results
```


## related codes

```
https://github.com/dcantrell/darwin-free
```
