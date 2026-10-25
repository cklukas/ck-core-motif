---
nav_exclude: true
---

# Accessibility UTF-8 allocation guards

Accessibility text conversion allocates a wide-character buffer and a UTF-8
result buffer.  The conversion now checks both host `size_t` limits and the
Xt `Cardinal` limit used by `XtMalloc`, and returns cleanly when either
allocation fails.

Valid conversions are unchanged, and no public ABI changes.  The static
regression check is `tests/automated/check-accessible-utf8-bounds.py`.
