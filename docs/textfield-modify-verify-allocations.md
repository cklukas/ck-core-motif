---
nav_exclude: true
---
# TextField modify-verify result allocations

When a modify-verify callback changes the proposed text, `TextField` allocates
the callback result in either multibyte or wide-character form.  Allocation
failure now rejects that edit and releases the temporary callback buffers
without dereferencing or returning a null result.

Regression coverage: `tests/automated/check-textfield-modify-verify-allocations.py`.
# TextField modify-verify result allocations

Modify-verify callback results now validate narrow-string lengths before
adding the terminating byte and narrowing the size for `XtMalloc`. This
matches the existing wide-string guards and keeps oversized callback output
on the failure path without truncating the allocation size.

Regression coverage: `tests/automated/check-textfield-modify-verify-allocations.py`.
