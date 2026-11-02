---
nav_exclude: true
---
# List hit-test edge bounds

`WhichItem` receives X event coordinates as `int`, but previously narrowed
them to legacy 16-bit `Position` before testing the row. On a large List that
could wrap an event coordinate and select a different row. It now keeps the
event coordinate wide, compares it directly with the widget height, and keeps
the subsequent row calculation in `long long` until it has proved the result
falls within the List's `int` item range.

The top-edge comparison also widens both operands before subtraction. Normal
row selection is unchanged, while large-window and transient geometry remain
deterministic. The contract is protected by
`tests/automated/check-list-hit-test-edge-bounds.py`.

The same audit found that `XmListSetHorizPos` stored its public `int` scroll
position directly in the legacy `Position` drawing origin. The scrollbar state
remains wide, while the X11 drawing coordinate is now explicitly clamped at
that representation boundary instead of wrapping.
