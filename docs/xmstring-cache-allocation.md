---
nav_exclude: true
---
# XmString cache allocation

Compound-string scanning and rendering caches are optional performance
structures.  Their constructors now check allocation before initializing or
linking a cache entry.  If memory is unavailable, cache lookup/set operations
fall back to their existing uncached behavior instead of dereferencing a null
entry; string content and rendering semantics are unchanged.

`tests/automated/check-xmstring-cache-allocation.py` protects this contract.
