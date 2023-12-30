# free for macOS

A memory usage utility for macOS  (similar with Linux Free Command)

This utility is designed to act like 'free' on Linux systems.  It is a tool for displaying memory usage statistics. Since the Mach microkernel design is different than Linux, the values reported are not an exact duplicate of what you can get from a Linux system.  Be sure to read the man page for an explanation of the program output.

# Installation

### HOMEBREW:

   A Homebrew formula is available at https://github.com/zfdang/homebrew-free-for-macOS
   
   To install via Homebrew, run the following:

      brew tap zfdang/free-for-macOS
      brew install free-for-macOS

### COMPILATION:

   You need a compiler, some headers, and a Mach-based system.  On Darwin,
   you can type make:

      make

   And out will pop a 'free' executable.  On other systems, it may require
   modification of the Makefile and/or source.

   Either run 

      make install

   or put the free executable in a public place and the man page in a sensible place.

# USAGE:

   See free(1).


# Reference

## how to calculate memory in macOS:

```
https://blog.guillaume-gomez.fr/articles/2021-09-06+sysinfo%3A+how+to+extract+systems%27+information
```

```
https://apple.stackexchange.com/questions/81581/why-does-free-active-inactive-speculative-wired-not-equal-total-ram
```

```
https://stackoverflow.com/questions/14789672/why-does-host-statistics64-return-inconsistent-results
```

```
https://novov.me/blog/posts/mixedmemories
```

```
https://www.jianshu.com/p/c31f63f14afb
```

## code and brew package

```
https://github.com/dcantrell/darwin-free
```
