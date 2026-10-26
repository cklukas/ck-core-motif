---
nav_exclude: true
---
# Drop-transfer name bounds

Drop-transfer objects receive internal names derived from a process-local
counter. The counter is unsigned and the name is written with `snprintf` into
the fixed widget-name buffer, avoiding signed overflow and unbounded writes.

The source contract is covered by
`tests/automated/check-drop-transfer-name-bounds.py`.
