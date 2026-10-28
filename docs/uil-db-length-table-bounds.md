---
nav_exclude: true
---

# UIL database name-length-table bounds

Name tables carry a separate signed length array. The loader now validates the
pointer-table capacity and `num_items + 1` arithmetic, rejects negative string
lengths, and bounds the accumulated serialized string bytes before allocation
and pointer rebasing.

Regression: `tests/automated/check-uil-db-length-table-bounds.py`
