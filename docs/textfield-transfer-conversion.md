---
nav_exclude: true
---

# TextField transfer conversion

TextField transfer and paste paths now preserve `mbstowcs` results as
`size_t`, reject failed or unrepresentable conversions, and explicitly
terminate successful wide buffers before replacement. Invalid transfer text
therefore fails safely instead of relying on a signed cast or exposing
unterminated storage.

Regression coverage:

```text
python3 tests/automated/check-textfield-transfer-conversion.py
```
