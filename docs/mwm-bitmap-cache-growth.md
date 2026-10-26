# MWM bitmap-cache growth

`GetBitmapIndex` grows each screen's bitmap cache in fixed-size increments.
Growth now checks both the counter increment and the byte multiplication before
calling Xt. It uses a temporary pointer for `XtRealloc`, so allocation failure
leaves the existing cache and counters intact instead of losing the live
pointer and resetting the cache state.

The allocation and failure-preservation contract is covered by
`tests/automated/check-mwm-image-path-allocation.py`.
