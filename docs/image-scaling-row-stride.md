# Image scaling row-stride rounding

The scaled-image path derives a temporary strip height by dividing a fixed
budget by the scaled source row stride.  For a very small positive scale,
floating-point rounding can produce a zero `scaled_row_bytes` even though the
rounding helper itself succeeded.  The scaler now rejects that result before
the division, preventing a divide-by-zero fault while preserving all normal
scales.

The focused source contract is checked by
`tests/automated/check-image-scaling-row-stride.py`.
