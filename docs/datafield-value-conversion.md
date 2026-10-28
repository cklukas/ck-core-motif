---
nav_exclude: true
---

# DataField value conversion

When constructing a wide-character DataField value, `mbstowcs` failure or an
unrepresentable result now releases the allocated buffer and resets the value
state before returning. The widget no longer publishes a negative string
length after invalid multibyte input.
The wide-string setter also keeps the `wcstombs` result in its native
`size_t` width before checking failure.

Regression coverage:

```text
python3 tests/automated/check-datafield-value-conversion.py
```
