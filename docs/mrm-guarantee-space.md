# Mrm record-growth bounds

`UrmCWR__GuaranteeSpace` now keeps the aligned delta in `size_t` until it has
been checked against `MrmMaxResourceSize` and the current record size. This
prevents full-word rounding from wrapping a 16-bit delta to zero and rejects
record growth that would exceed the format limit.

The source-level regression is `tests/automated/check-mrm-guarantee-space.py`.
