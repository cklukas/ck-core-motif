---
nav_exclude: true
---

# DataField replacement conversion

`XmDataFieldReplace` now rejects input that cannot be represented by its
legacy signed character-count API or that fails `mbstowcs`. It releases the
temporary wide buffer and leaves replacement unsuccessful instead of silently
publishing an empty replacement.

Regression coverage:

```text
python3 tests/automated/check-datafield-replace-conversion.py
```
