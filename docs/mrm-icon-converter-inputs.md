# Mrm icon converter input validation

`UrmCreatePixmap` and `UrmCreateBitmap` consume icon records loaded from UID
resources and return an X pixmap through a caller-owned pointer. Both helpers
now reject a missing icon or result pointer before reading the record, and
clear a valid result pointer before conversion begins.

The previous code dereferenced `icon` immediately and passed `pixmap` to lower
mapping routines without checking it. A malformed resource or invalid internal
call could therefore crash instead of returning `MrmFAILURE`. Successful
conversion and format validation are unchanged. The source-level regression is
`tests/automated/check-mrm-icon-converter-inputs.py`.
