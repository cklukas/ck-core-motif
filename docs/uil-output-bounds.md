---
nav_exclude: true
---
# UIL generated output bounds

The UIL-to-URM emitter creates fallback widget names from source coordinates
and formats URM failures with the latest error text.  Both values can exceed
the small legacy stack buffers, especially for long diagnostic messages.
These paths now use `snprintf`; the fallback name buffer is sized for the
complete coordinate form and the diagnostic remains safely truncated at its
existing 132-byte boundary.

`tests/automated/check-uil-output-bounds.py` protects both formatting paths.
