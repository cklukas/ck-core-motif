# Text input edge bounds

Text pointer-focus and autoscroll handling now widens widget dimensions and
text margins to `int64_t` before subtracting them. A very small text widget
can legitimately have margins larger than its current allocation while it is
being resized. Subtracting an `int` margin from an unsigned X11 `Dimension`
first can wrap to a large positive coordinate, sending focus or autoscroll to
the wrong edge.

The edge coordinates are now kept in signed wide arithmetic and converted
through the existing `ClampTextPointCoordinate` helper before being written
back to an X11 `Position`. This preserves normal Motif behavior while making
tiny and transient allocations deterministic.

The source contract is checked by:

```text
python3 tests/automated/check-text-input-edge-bounds.py
```
