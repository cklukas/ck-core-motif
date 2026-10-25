---
nav_exclude: true
---
# Virtual-key result allocation

`XmeVirtualToActualKeysyms` returns a newly allocated array of concrete
keysyms for a virtual key. It now rejects a null output pointer and checks the
result allocation before filling it, returning zero matches on failure.

Successful mappings preserve their ordering and count.
