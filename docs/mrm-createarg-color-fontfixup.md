# Mrm color and font reader boundaries

The special color handling in `Urm__CW_CreateArglist` now skips a missing color
descriptor safely, and `Urm__CW_FixupValue` rejects null font records or
buffers before swapping offsets. Valid color and font resources retain their
existing conversion behavior.

The source-level regression check is
`tests/automated/check-mrm-createarg-color-fontfixup.py`.
