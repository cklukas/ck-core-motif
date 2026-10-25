# Shared shadow origins

`DrawSimpleShadow` builds the segment geometry used by `XmeDrawShadows` and
separator shadows. Its old expressions combined signed `Position` origins with
unsigned `Dimension` sizes directly. A negative or off-edge origin could wrap
before the value was assigned to an XSegment coordinate.

The helper now performs origin and size arithmetic in signed `int` locals and
casts only when storing the final XSegment coordinates. Shadow thickness
clamping and the public drawing API remain unchanged. The regression
`tests/automated/check-draw-shadow-origins.py` covers the conversion boundary.
