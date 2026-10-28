---
nav_exclude: true
---

# TextField drawing conversion

Wide TextField drawing and measurement now calculate conversion capacities with
checked `size_t` arithmetic, reject `wcstombs` failure, and only cast byte
lengths to the legacy Xlib/Xft `int` APIs after bounds checking. Invalid text
is skipped instead of being rendered with a wrapped length.

Regression coverage:

```text
python3 tests/automated/check-textfield-drawing-conversion.py
```
