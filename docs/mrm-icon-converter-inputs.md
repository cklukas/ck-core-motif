---
nav_exclude: true
---
# Mrm icon converter input validation

`UrmCreatePixmap` and `UrmCreateBitmap` consume icon records loaded from UID
resources and return an X pixmap through a caller-owned pointer. Both helpers
now reject a missing icon or result pointer before reading the record, and
clear a valid result pointer before conversion begins. They also reject a
nonpositive width or height and a missing in-memory pixel buffer before the
mapping routines perform stride arithmetic or pass dimensions to Xlib.

The previous code dereferenced `icon` immediately and passed `pixmap` to lower
mapping routines without checking it. A malformed resource or invalid internal
call could therefore crash instead of returning `MrmFAILURE`. Successful
conversion and format validation are unchanged; malformed records continue to
use the established `MrmNOT_VALID` result. The source-level regression is
`tests/automated/check-mrm-icon-converter-inputs.py`.
