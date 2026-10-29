# Container none-style placement bounds

`XmContainer` none-style placement now computes its usable right and bottom
edges in signed 64-bit arithmetic. If a transient container allocation is
smaller than its margins, subtracting the margins from an unsigned
`Dimension` previously wrapped and could mark an out-of-bounds child as
inside the container.

Child origins and extents are widened before comparison, preserving ordinary
placement while making undersized containers deterministic.

The source contract is checked by:

```text
python3 tests/automated/check-container-place-none-bounds.py
```
