---
nav_exclude: true
---

# UIL database integer-table bounds

Enumeration-set records contain a variable number of unsigned-short values.
The loader now validates the descriptor-table capacity, rejects negative
counts, and accumulates the value count in `size_t` with an Xt allocation
bound before reading or rebasing the values.

Regression: `tests/automated/check-uil-db-int-table-bounds.py`
