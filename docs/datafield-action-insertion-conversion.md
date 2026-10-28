---
nav_exclude: true
---

# DataField action insertion conversion

Keyboard and action-table insertion now preserve the native `size_t` result
from `mbstowcs`, reject conversion failure or lengths too large for Motif's
signed text-position APIs, and only then pass a checked `int` length to the
replacement routine. Key lookup also rejects a result that fills the entire
temporary buffer, preserving room for the required NUL terminator.

Regression coverage:

```text
python3 tests/automated/check-datafield-action-insertion-conversion.py
```
