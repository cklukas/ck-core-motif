---
nav_exclude: true
---

# TextField modify-verify conversion

The TextField modify-verify bridge now preserves the native `size_t` results
from `wcstombs` and `mbstowcs`. Failed conversions and lengths that cannot fit
Motif's signed callback fields are rejected or converted to an empty callback
payload instead of being tested as negative `int` values and wrapped.

Regression coverage:

```text
python3 tests/automated/check-textfield-modify-verify-conversion.py
```
