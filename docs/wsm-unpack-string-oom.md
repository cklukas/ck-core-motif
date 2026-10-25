---
nav_exclude: true
---

# WSM string unpack allocation

The WSM protocol unpacker allocates storage for length-prefixed strings.
`UnpackString` now checks that allocation before writing the decoded bytes and
returns `NULL` on failure.  This preserves the wire format while preventing a
null-pointer write when a process is under memory pressure; existing callers
can propagate their normal allocation-failure handling.
