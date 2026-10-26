---
nav_exclude: true
---

# TabStack navigation API inputs

The public TabStack navigation helpers now reject null widget handles before
walking parent relationships or entering the application lock. Invalid
optional handles return the existing no-op or `NULL` result; valid tab
selection and child lookup behavior is unchanged.

The focused regression check is `tests/automated/check-tabstack-inputs.py`.
