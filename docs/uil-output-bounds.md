---
nav_exclude: true
---
# UIL generated output bounds

The UIL-to-URM emitter creates fallback widget names from source coordinates
and formats URM failures with the latest error text.  Both values can exceed
the small legacy stack buffers, especially for long diagnostic messages.
The fallback name uses bounded `snprintf`.  For URM failures, the emitter first
measures the complete text, validates the resulting size, allocates exactly
that many bytes plus its terminator, and frees the temporary string after
delivering it to the diagnostic path.  Diagnostic detail is therefore retained
without a fixed 132-byte truncation boundary.

`tests/automated/check-uil-output-bounds.py` protects both formatting paths.
