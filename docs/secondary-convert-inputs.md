---
nav_exclude: true
---

# Secondary conversion callback inputs

`SecondaryConvertHandler` receives a selection-conversion callback and an
`ATOM_PAIR` parameter payload.  It now refuses callbacks with no widget or
callback structure, and refuses a missing or undersized pair before reading
the selection request or casting the payload.  Valid requests continue to
use the existing synchronous secondary-selection flow.

The regression check is
`tests/automated/check-secondary-convert-inputs.py`.
