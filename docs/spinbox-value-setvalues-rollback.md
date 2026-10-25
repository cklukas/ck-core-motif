---
nav_exclude: true
---
# SpinBox value-list SetValues rollback

SpinBox now keeps the current string-child value list when a replacement array
or any of its `XmString` copies cannot be allocated during `SetValues`.  A
partial replacement is cleaned up, and the old list is not freed, preventing
NULL entries and preserving the current text/value behavior.

The focused regression is
`tests/automated/check-spinbox-value-setvalues-rollback.py`.
