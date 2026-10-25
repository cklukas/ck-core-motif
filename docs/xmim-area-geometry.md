# Input-method area geometry bounds

The input-method integration now clamps shell-height removal and status or
preedit rectangle placement when the IM area is taller than its shell.  This
prevents unsigned `Dimension` subtraction from wrapping shell requests or XIM
area coordinates during provider teardown and reconfiguration.

The source-level regression is `tests/automated/check-xmim-area-bounds.py`.
