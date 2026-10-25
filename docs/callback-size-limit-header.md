---
nav_exclude: true
---
# Callback size-limit header

The internal callback-list growth guard uses `SIZE_MAX` to reject allocation
sizes that cannot be represented. `CallbackI.c` now includes `<stdint.h>`, the
standard header that defines that macro on supported C implementations, so the
guard compiles consistently on platforms where `<limits.h>` does not provide
it.

The focused regression is
`tests/automated/check-callback-size-limit-header.py`.
