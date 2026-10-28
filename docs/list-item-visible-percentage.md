---
nav_exclude: true
---

# List keyboard-visibility percentage

The `ListItemVisible` translation action now rejects malformed or out-of-range
parameters, clamps valid percentages to 0–100, and computes the visible-item
offset without multiplying the full item count by the percentage first.

The focused regression check is
`tests/automated/check-list-item-visible-percentage.py`.
