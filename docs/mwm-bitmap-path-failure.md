# MWM bitmap-path failures

`GetBitmapIndex` reserves a null or empty bitmap name for the built-in icon.
A non-empty name whose path cannot be represented or resolved no longer shares
that null result: MWM reports the requested bitmap as unavailable and returns
failure.  Consequently a too-long configured bitmap path cannot silently
replace the requested icon with the built-in one.

The focused source contract is covered by
`tests/automated/check-mwm-bitmap-path-failure.py`.
