---
nav_exclude: true
---

# UIL database string-table bounds

Key-table database records contain a serialized table followed by variable
length names. The loader now validates the record capacity, accumulates name
storage in `size_t` with an Xt-`Cardinal` bound, and checks allocation before
reading or rebasing string pointers.

Regression: `tests/automated/check-uil-db-string-bounds.py`
