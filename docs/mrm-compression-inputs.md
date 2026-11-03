---
nav_exclude: true
---
# Mrm compression-table input checks

The compression helpers are internal boundaries shared by widget creation and
resource-reference handling. They now reject null file, output, and return
pointers before dereferencing them, and reject table entries that do not contain
a string or class descriptor. Subtree checks return false for an unavailable or
invalid table. Valid compressed resource and class lookups keep their existing
results and diagnostics.

Class-descriptor and resource-name decompression lookups now clear their
validated output pointers before examining the requested table entry.  A
missing table, invalid code, or empty entry cannot leave a stale class or
string pointer in the caller's output.
