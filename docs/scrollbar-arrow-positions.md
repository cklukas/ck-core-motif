# ScrollBar far-side arrow positions

Far-side arrow coordinates now use signed intermediates before conversion to
the X toolkit `Position` type.  When a collapsed scrollbar cannot fit its
insets and arrow extent, the coordinate remains a valid negative/off-edge
position instead of wrapping to a distant positive location.

The source-level regression is
`tests/automated/check-scrollbar-arrow-positions.py`.
