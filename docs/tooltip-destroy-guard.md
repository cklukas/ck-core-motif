---
nav_exclude: true
---

# Tooltip label teardown guard

The optional tooltip label-destroy callback now ignores a missing callback
widget and avoids destroying a missing parent shell. Normal tooltip label and
shell teardown remains unchanged.

The focused source-contract check is
`python3 tests/automated/check-tooltip-destroy-guard.py`.
