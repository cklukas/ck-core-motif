---
nav_exclude: true
---

# MWM session database-name allocation

MWM reserves extra capacity for the numeric suffix appended when it writes
successive session database files. Command-line and resource-provided names
now use one checked copy helper before that spare capacity is added. An
unrepresentable length or allocation failure leaves the existing fallback path
available instead of wrapping the allocation size before copying the name.

The public session database naming behavior is unchanged for representable
paths. The focused regression check is
`tests/automated/check-mwm-db-name-allocation.py`.
