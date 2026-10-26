---
nav_exclude: true
---
# Dialog zero-argument construction

`XmeCreateClassDialog` now copies the caller’s argument list only when
`bb_n` is nonzero. A zero-argument dialog can therefore pass a null argument
list without invoking zero-length `memcpy` on a null pointer, while non-empty
argument lists retain the same behavior.

The focused regression is `tests/automated/check-dialog-zero-args.py`.
