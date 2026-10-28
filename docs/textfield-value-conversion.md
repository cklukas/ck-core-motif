---
nav_exclude: true
---

# TextField value conversion

TextField value validation now preserves native conversion results and checks
allocation bounds before publishing either wide or narrow storage. Failed
`mbstowcs` conversions reset the wide value safely; failed or
capacity-filling `wcstombs` conversions release the narrow buffer instead of
exposing unterminated text.

Regression coverage:

```text
python3 tests/automated/check-textfield-value-conversion.py
```
