---
nav_exclude: true
---
# TextField SetValues empty wide value

When a multibyte TextField receives a null wide value through `SetValues()`,
the replacement terminator allocation is now checked before initialization.
Failure rejects the update after restoring insertion-point painting.

Regression coverage: `tests/automated/check-textfield-setvalues-empty.py`.
