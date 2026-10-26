# Image depth sentinel bounds

Motif accepts negative image depths as a compatibility convention for bitmap
and pixmap selection. `INT_MIN` cannot be negated in a signed `int`, so
`_XmGetScaledPixmap` now rejects it before normalization. Other valid depth
values retain their existing behavior.

The focused source contract is checked by
`tests/automated/check-image-depth-sentinel.py`.
