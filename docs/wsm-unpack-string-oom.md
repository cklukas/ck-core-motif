---
nav_exclude: true
---

# WSM string unpack allocation

The WSM protocol unpacker allocates storage for length-prefixed strings.
`UnpackString` now checks both the decoded length and Xt's `Cardinal` width
before allocation, then checks the allocation before writing decoded bytes. It
returns `NULL` on failure, preserving the wire format while preventing narrow
size conversion and null-pointer writes; existing callers can propagate their
normal allocation-failure handling.
