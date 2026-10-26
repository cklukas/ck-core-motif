---
nav_exclude: true
---
# MWM title-width bounds

`GetFramePartInfo` checks the total horizontal space required by frame borders
and title-bar gadgets before calculating `FRAME_TITLE` or (when enabled)
`FRAME_TITLEBAR`.  If the frame is too narrow, the function returns `FALSE`
without writing a wrapped unsigned width.  Valid frames retain the previous
geometry calculations.

The source-contract regression is
`tests/automated/check-mwm-title-width-bounds.py`.
