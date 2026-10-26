# XPM pixel-map growth bounds

The XPM image scanner grows its distinct-pixel table as new colors are found.
The growth path now checks both the slot-count doubling and the resulting
`sizeof(Pixel)` allocation size before calling `XpmRealloc`.  If either value
cannot be represented, scanning fails cleanly instead of wrapping the size and
writing beyond the resized table.

Normal images keep the existing doubling behavior.  The focused source-contract
check is:

```sh
python3 tests/automated/check-xpm-scan-growth-bounds.py
```
