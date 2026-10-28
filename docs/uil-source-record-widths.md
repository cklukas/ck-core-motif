---
nav_exclude: true
---

# UIL source-record width validation

Source diagnostics and machine-code records retain compact offsets, lengths,
and source positions for compatibility. The append paths now reject values
outside those field widths and reject allocations that cannot fit Xt's
`Cardinal` size before narrowing or copying.
File-position keys are also range-checked before crossing the legacy unsigned
key/`fseek` signed-offset boundary, and `ftell` failures no longer become huge
unsigned positions.

Regression: `tests/automated/check-uil-source-record-widths.py`
