# Region allocation size bounds

`_XmRegionCreateSize` allocates an array of `XmRegionBox` records from a
caller-supplied `long` count.  The old implementation cast that count to the
Xt `Cardinal` allocation size without checking negative values, narrowing, or
multiplication overflow.

The function now rejects non-positive counts and counts that cannot be
represented by the allocation API after multiplying by `sizeof(XmRegionBox)`.
Normal region creation is unchanged, while malformed or extreme geometry
requests fail cleanly.  The source regression in
`tests/automated/check-region-size-bounds.py` protects the validation.
