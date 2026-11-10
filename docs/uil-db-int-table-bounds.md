---
nav_exclude: true
---

# UIL database integer-table bounds

Enumeration-set records contain a variable number of unsigned-short values.
The loader validates the descriptor-table capacity, rejects negative counts,
and accumulates the value count in `size_t` with an Xt allocation bound.  It
then computes the integer-table byte count once and uses that exact value for
both allocation and input.  This keeps the allocation unit explicit and avoids
repeating unchecked element-to-byte arithmetic before reading or rebasing the
values.

Regression: `tests/automated/check-uil-db-int-table-bounds.py`
