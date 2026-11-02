# Scale scrollbar arithmetic bounds

`XmScale` translates its available scrollbar span into the scrollbar child's
integer slider size.  The span includes highlight and shadow thickness, and
the slider calculation uses the large `SCROLLBAR_MAX` coordinate range.

The span and `SCROLLBAR_MAX / span * slider-size` product are now held in
`int64_t`.  The result is clamped to the established `[1, SCROLLBAR_MAX]`
contract before it is returned through the legacy integer scrollbar API.  This
preserves normal sizes while preventing a large configured Scale from wrapping
before the existing bounds policy can apply.

When the slider occupies the full coordinate range, its input callback now
recognizes that there is no representable movement and leaves the current Scale
value unchanged instead of dividing by a zero scrollbar travel range.

`tests/automated/check-scale-scrollbar-arithmetic.py` guards this conversion
boundary.
