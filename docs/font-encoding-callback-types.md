---
nav_exclude: true
---
# Font selector encoding callback indexes

Font selector encoding menu callbacks carry a small integer index through
Xt's `XtPointer` callback data slot.  The old implementation cast directly
between pointers and `int`, which emits 64-bit diagnostics and relies on a
non-portable conversion.

The callback producer and consumer now convert through `uintptr_t`.  The
index values and callback behavior remain unchanged, while the code is
well-defined on 32-bit and 64-bit builds.  The source regression in
`tests/automated/check-font-encoding-callback-types.py` protects both sides
of the conversion.
