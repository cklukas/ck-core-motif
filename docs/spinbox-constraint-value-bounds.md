---
nav_exclude: true
---
# SpinBox constraint value-table bounds

SpinBox string-child constraints now validate the requested value count before
allocating an `XmString` table. Counts that cannot be represented in either
`size_t` or Xt's historical `Cardinal` byte-size argument are treated as
allocation failures and follow the existing rollback path.

The focused regression is
`tests/automated/check-spinbox-value-init-rollback.py`.
