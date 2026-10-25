---
nav_exclude: true
---
# List selection-callback snapshot rollback

List selection callbacks now copy their selected-item snapshots
transactionally. If any `XmStringCopy` fails, already-copied entries and the
temporary position array are released and the callback reports zero selected
items rather than exposing NULL entries under a nonzero count.

The focused regression is
`tests/automated/check-list-selection-callback-allocation.py`.
