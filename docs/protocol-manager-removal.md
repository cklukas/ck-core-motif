---
nav_exclude: true
---

# Protocol-manager teardown

Protocol-manager removal now validates membership before freeing a manager,
compacts the owner list, and decrements `num_protocol_mgrs`. Destroy-time
teardown removes the first remaining manager until the list is empty, avoiding
stale pointers and skipped entries.

The focused regression check is
`tests/automated/check-protocol-manager-removal.py`.
