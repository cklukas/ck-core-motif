---
nav_exclude: true
---
# MWM title-width bounds

`GetFramePartInfo` checks the total horizontal space required by frame borders
and title-bar gadgets before calculating title, system-menu, minimize, or
maximize geometry.  If the frame is too narrow, the function returns `FALSE`
without writing wrapped unsigned widths or x-coordinates.  Valid frames retain
the previous geometry calculations.

The source-contract regression is
`tests/automated/check-mwm-title-width-bounds.py`.
