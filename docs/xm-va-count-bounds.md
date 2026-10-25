---
nav_exclude: true
---
Simple-menu varargs counter bounds
==================================

The nested-list and top-level varargs counters now saturate at `INT_MAX`
instead of overflowing signed counters.  The public counting helper also
returns safely when an output pointer is missing.  Saturated counts are then
rejected by the constructor arithmetic and allocation checks, preserving a
fail-closed result for malformed or unrepresentably large lists.
