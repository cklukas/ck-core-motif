---
nav_exclude: true
---
# TextField modify-verify result allocations

When a modify-verify callback changes the proposed text, `TextField` allocates
the callback result in either multibyte or wide-character form.  Allocation
failure now rejects that edit and releases the temporary callback buffers
without dereferencing or returning a null result.

Regression coverage: `tests/automated/check-textfield-modify-verify-allocations.py`.
