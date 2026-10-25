# Frame shadow redisplay guards

`XmFrame` caches the previous shadow rectangle so it can repaint a frame
around title and work-area changes. During realization, resource replacement,
or a zero-sized geometry transition, that cached state can be incomplete.
`DrawShadow` now requires both manager shadow GCs, non-zero cached dimensions,
and a non-zero shadow thickness before calling `XmeDrawShadows`.

This only suppresses an invalid intermediate draw. Realized frames with a
valid rectangle continue to use the existing shadow type and geometry, and
frame title/gadget expose processing is unchanged.

The focused source contract is checked by
`tests/automated/check-frame-shadow-redisplay-guards.py`.
