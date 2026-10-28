---
nav_exclude: true
---
# Mrm class-registration allocation bounds

Class registration stores the creator name immediately after its descriptor.
The allocation now checks the terminator-inclusive name length and descriptor
addition before calling `XtMalloc`, including Xt's `Cardinal` width on
platforms where `size_t` is wider. This preserves normal registration and
lookup behavior while rejecting an unrepresentable allocation safely.

The source-level regression is
`tests/automated/check-mrm-wci-registration-bounds.py`.
