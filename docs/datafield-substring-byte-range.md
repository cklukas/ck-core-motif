# DataField byte-substring range validation

`XmDataFieldGetSubstring` now validates the output buffer, start, and length
before forming a source pointer. It also truncates an overlong valid request
before calculating the byte count and rejects byte-count/buffer-size overflow.
The narrow and wide substring APIs therefore share the same malformed-range
behavior.

The regression check is:

```sh
python3 tests/automated/check-datafield-substring-byte-range.py
```
