---
nav_exclude: true
---

# Clipboard format-atom allocation size

`ClipboardGetAtomFromFormat` now rejects combined prefix/name lengths that do
not fit Xt’s `Cardinal` allocator size before converting and allocating them.
The existing `size_t` overflow check remains in place.

Regression: `tests/automated/check-clipboard-format-atom-size.py`
