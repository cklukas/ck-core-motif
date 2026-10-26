# DataField wide-substring output validation

`XmDataFieldGetSubstringWcs` now rejects a null output buffer and non-positive
buffer size before copying. Its capacity check also avoids `num_chars + 1`
signed overflow by comparing the requested character count directly.

The regression check is:

```sh
python3 tests/automated/check-datafield-substring-wcs-buffer.py
```
