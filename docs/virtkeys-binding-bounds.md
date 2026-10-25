---
nav_exclude: true
---
# Virtual-key binding file growth

`_XmVirtKeysLoadFileBindings` reads a binding file in 255-byte chunks and
accumulates the result for the display. The old byte counter used `int` and
added only one fixed growth block, which could wrap or leave the destination too
small for a large valid file.

The loader now uses `size_t` arithmetic, grows repeatedly until the complete
line fits, limits allocations to the Xt allocator's representable size, and
returns failure cleanly when the input cannot be represented. Existing comment
handling and binding text are unchanged.

`tests/automated/check-virtkeys-binding-growth.py` guards the growth contract.
