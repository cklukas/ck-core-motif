---
nav_exclude: true
---
# XmString unoptimization allocation

The non-destructive `Unoptimize` path now checks its segment-array size and
each converted/copied segment.  Failed conversion releases all partial
segments and the new array entry, returning `NULL` without modifying the
source string.
