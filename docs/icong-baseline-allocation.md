# IconG baseline allocation

IconG baseline queries allocate a one-entry `Dimension` array for the
calculated baseline.  The allocation is now checked before the value is
written; failure returns zero lines and a null baseline result.

The focused source contract is checked by
`tests/automated/check-icong-baseline-allocation.py`.
