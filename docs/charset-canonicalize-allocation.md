---
nav_exclude: true
---

# Legacy charset canonicalization allocation

The compatibility charset canonicalization helper now checks allocation for
ASCII, ISO, and pass-through charset results before writing the output string.
On allocation failure it returns `NULL`, matching the helper's existing
failure contract without changing successful canonicalization.

The focused regression check is
`tests/automated/check-charset-canonicalize-allocation.py`.
