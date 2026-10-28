---
nav_exclude: true
---

# UIL integer-to-keysym range

Private integer values coerced to a keysym are serialized as one character.
The validator now accepts only values in the unsigned-byte range before
allocating and copying that character; out-of-range values remain invalid
instead of silently truncating through a `char` conversion.

Regression: `tests/automated/check-uil-keysym-range.py`
