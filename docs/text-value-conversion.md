---
nav_exclude: true
---

# Text value conversion

Text widget initialization now treats the `wcstombs` result as `size_t` and
rejects conversion failure before indexing the temporary buffer. This avoids
the ineffective signed `>= 0` check and only publishes a NUL-terminated value.

Regression coverage:

```text
python3 tests/automated/check-text-value-conversion.py
```
