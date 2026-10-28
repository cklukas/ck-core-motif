---
nav_exclude: true
---

# UIL resource-vector allocation bounds

UIL initializes several unsigned-short resource tables from limits read from
the database. Their byte counts now pass through one checked helper before
conversion to Xt's `Cardinal` allocation width. Negative or unrepresentable
limits stop through Xt's normal fatal-error path instead of wrapping an
allocation size.

Regression: `tests/automated/check-uil-vector-allocation-bounds.py`
