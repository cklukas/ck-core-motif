---
nav_exclude: true
---

# Editres payload allocation

The Editres protocol helpers now check the terminator-inclusive wire-string
size against Xt's `Cardinal` width, as well as checking allocations for
decoded strings and widget-ID arrays before writing payload bytes. A rejected
size or failed allocation reports a protocol parse failure to the existing
cleanup path; zero-length widget lists remain valid.

The focused regression check is
`tests/automated/check-editres-payload-allocation.py`.
