# MWM bitmap-cache index bounds

`GetBitmapIndex` returns an `int`, while the per-screen bitmap-cache count is
unsigned. It now rejects a null screen record and any cache count that cannot
be represented by the return type before searching or appending. This prevents
large-cache indices from silently wrapping to negative values at the API
boundary.

The guard is covered by
`tests/automated/check-mwm-image-path-allocation.py`.
