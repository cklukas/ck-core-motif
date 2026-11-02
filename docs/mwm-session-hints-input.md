# MWM DT session-hint input

`GetDtSessionHints` now returns immediately when called without a
`WmScreenData` record. This prevents a null dereference before the root-window
property lookup. Calls with valid screen data continue to parse and remove the
session-hints property as before. The item-count boundary is also checked
before allocation: null screen data, zero or negative counts, and counts that
cannot fit in the Xt `Cardinal` allocation size are rejected without touching
the destination. The allocation byte count is computed once in `size_t` and
reused for zero-initialization, so allocation and initialization cannot
disagree.

The bounded X11 property read must be complete before its bytes are copied into
the session parser. A property with bytes remaining is discarded with the same
restart cleanup as any other unusable session hint.
