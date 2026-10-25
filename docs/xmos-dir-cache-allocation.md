---
nav_exclude: true
---
# Xmos directory-cache allocation

The file-selection directory cache grows in fixed blocks and stores a copy of
each entry name. Growth now uses a temporary pointer and publishes the new
capacity only after `XtRealloc` succeeds; entry-size arithmetic and entry
allocation are checked before the cache is indexed.

A failed cache update returns the existing no-match result without corrupting
the cache or dereferencing a null entry.
