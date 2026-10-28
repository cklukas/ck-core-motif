---
nav_exclude: true
---

# DataField wide-value export

When a wide DataField value is rebuilt into its narrow storage, the
`wcstombs` result is now checked before the buffer becomes the widget's live
value. Conversion failure and output that fills the allocation are rejected,
the owned buffer is released, and the value is reset instead of exposing
unterminated or partially converted text.

Regression coverage:

```text
python3 tests/automated/check-datafield-wide-value-export.py
```
