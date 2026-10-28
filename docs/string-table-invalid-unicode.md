---
nav_exclude: true
---

# String-table conversion rejects invalid multibyte input

`CvtStringToStringTable` now treats a negative `mblen` result as a conversion
failure. Previously it could stop at the invalid byte, publish the valid
prefix, and silently discard the remainder. The failure path releases the
already duplicated entries and leaves the destination conversion unset;
valid comma-separated and escaped-comma values retain their existing behavior.

The focused source-contract check is
`tests/automated/check-string-table-converter-allocation.py`.
