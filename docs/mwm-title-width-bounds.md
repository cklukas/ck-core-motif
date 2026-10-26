---
nav_exclude: true
---
# MWM title-width bounds

`GetFramePartInfo` checks the total horizontal space required by frame borders
and title-bar gadgets before calculating title, system-menu, minimize, or
maximize geometry.  If the frame is too narrow, the function returns `FALSE`
without writing wrapped unsigned widths or x-coordinates.  Valid frames retain
the previous geometry calculations.

Resize-corner spans use the same checked subtraction: when twice the corner
size exceeds the frame dimension, the interior span is zero rather than a
wrapped unsigned value.

East and south edge placement also rejects frames smaller than the lower
border, preventing wrapped edge coordinates.

Northeast and southeast corner coordinates clamp to zero when their computed
span exceeds the corresponding frame dimension.

The title child itself now rejects an initial frame narrower than both upper
borders, and later reconfiguration skips that invalid subtraction.

The source-contract regression is
`tests/automated/check-mwm-title-width-bounds.py`.
