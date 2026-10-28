---
nav_exclude: true
---
# TextField unsupported-character warning allocation

The diagnostic path used when a TextField encounters an unsupported
multibyte character now checks the escaped-warning buffer size before
computing `4 * num_conv` or passing it to `XtMalloc`.  This prevents signed
arithmetic overflow and truncation of the allocator's `Cardinal` size while
preserving the normal warning for representable input.

Regression coverage: `tests/automated/check-textfield-warning-allocation.py`.
