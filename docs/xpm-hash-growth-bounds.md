# XPM hash-table growth bounds

The XPM color-key hash table doubles its slot count when it reaches its load
limit.  Growth now rejects a zero or already-maximal slot count before the
doubling macro runs, preventing unsigned wraparound from turning an oversized
table into a small allocation.  The existing post-growth allocation-size
check remains in place.

Valid tables retain the same growth policy; an unrepresentable growth returns
`XpmNoMemory`.

The focused source-contract check is:

```sh
python3 tests/automated/check-xpm-hash-growth-bounds.py
```
