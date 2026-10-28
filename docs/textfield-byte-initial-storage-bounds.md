---
nav_exclude: true
---
# TextField byte initial-storage bounds

TextField validation now checks the byte-string length before assigning it to
the signed `string_length` field or adding the initial growth reserve. The
allocation is narrowed to Xt's `Cardinal` only after the range checks, matching
the wide-character initialization path and preventing oversized input from
wrapping into a small buffer.

Regression coverage: `tests/automated/check-textfield-byte-initial-storage-bounds.py`.
