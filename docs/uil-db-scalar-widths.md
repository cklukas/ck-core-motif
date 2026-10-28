---
nav_exclude: true
---

# UIL database scalar widths

Database reads return `size_t` counts while several UIL globals and generated
identifiers use compact legacy types. The loader now makes those bounded
conversions explicit at the `fread`, charset-count, and user-defined-object
boundaries.

Regression: `tests/automated/check-uil-db-scalar-widths.py`
