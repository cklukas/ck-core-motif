# MWM group-argument bounds

MWM group parsing now keeps the scanned pointer distance in `size_t` and
clamps it directly to the fixed group buffer capacity. This avoids narrowing
the distance through the generic integer `min` macro while preserving the
existing accepted group names and truncation limit.

The source contract is covered by
`tests/automated/check-wm-resparse-group-length.py`.
