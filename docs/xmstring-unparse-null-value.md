---
nav_exclude: true
---
# XmString unparse null values

The unparse helper now ignores malformed non-empty components with null value
pointers before conversion or copying. This avoids a crash while retaining the
existing partial-output behavior used for allocation and conversion failures.

The regression check is
`tests/automated/check-xmstring-unparse-null-value.py`.
