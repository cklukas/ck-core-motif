---
nav_exclude: true
---

# TextField preedit conversion

Input-method preedit updates now validate all multibyte/wide conversions before
using their buffers. Converted strings are explicitly terminated, and invalid
recovery text is discarded instead of being concatenated from uninitialized
storage or being accepted through a signed `< 0` check on a `size_t` result.

Regression coverage:

```text
python3 tests/automated/check-textfield-preedit-conversion.py
```
