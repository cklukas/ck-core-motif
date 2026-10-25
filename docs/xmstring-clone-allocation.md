---
nav_exclude: true
---
# XmString clone allocation

Non-optimized `XmStringCopy` clones now check the entry-array size and every
entry copy.  A failed clone releases all entries copied so far and the new
top-level string, returning `NULL` without publishing a partially initialized
compound string.
