---
nav_exclude: true
---
# XmString entry-copy index width

Compound `XmString` entries store their segment count in a size-compatible
allocation path.  Entry copying now uses a `size_t` loop index for that count,
instead of narrowing it to `int` after allocation.  This keeps large
representable segment arrays fully traversable while preserving normal copy and
cleanup behavior.

`tests/automated/check-xmstring-entry-index-width.py` protects this contract.
