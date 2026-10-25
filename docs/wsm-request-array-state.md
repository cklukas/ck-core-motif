---
nav_exclude: true
---

# WSM request array state

Request packing now validates positive counts and nested pointers for
`CONNECT`, `SET_STATE`, and `WM_WINDOWS` requests before sizing or indexing
their arrays.  Inconsistent application-provided state returns `NULL`; valid
zero-count and fully populated requests retain their existing wire format.
