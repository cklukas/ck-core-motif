---
nav_exclude: true
---

# Screen child insertion input validation

Screen child insertion now validates the child, parent capacity, and backing
list before growing or writing the child array. Valid insertion and growth
behavior remain unchanged.

The focused regression check is
`tests/automated/check-screen-insert-child-input.py`.
