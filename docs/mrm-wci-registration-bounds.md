---
nav_exclude: true
---
# Mrm class-registration allocation bounds

Class registration stores the creator name immediately after its descriptor.
The allocation now checks the terminator-inclusive name length and descriptor
addition before calling `XtMalloc`, preserving normal registration and lookup
behavior while rejecting an unrepresentable allocation safely.

The source-level regression is
`tests/automated/check-mrm-wci-registration-bounds.py`.
