---
nav_exclude: true
---
# String-table conversion capacity

The resource converter grows a `String *` table while parsing comma-separated
resource values.  Its old growth guard considered only the `int` element
count; the later `count * sizeof(String)` expression is passed through Xt's
`Cardinal` allocation interface and could wrap first on 64-bit builds.

Growth and final termination now check the element count against the actual
allocation type before multiplying.  Valid resource tables retain their
existing parsing and ordering, while unrepresentable tables fail cleanly.
The source regression in `tests/automated/check-string-table-capacity.py`
protects the growth and final-size guards.
