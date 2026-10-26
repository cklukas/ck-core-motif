---
nav_exclude: true
---
# XPM file-buffer arguments

The convenience APIs for reading a file into a buffer and writing a buffer to
a file now reject null filenames, output pointers, and input buffers before
calling libc file functions.

The source contract is covered by
`tests/automated/check-xpm-file-buffer-arguments.py`.
