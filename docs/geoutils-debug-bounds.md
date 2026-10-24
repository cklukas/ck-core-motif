# GeoUtils debug-label bounds

The optional geometry diagnostics prepend caller-supplied headers to list and
matrix coordinates.  The old debug-only code used `sprintf` into 256-byte
stack buffers, so a long header could corrupt the diagnostic process.

Both debug label formats now use `snprintf` with the destination size.  The
diagnostic text is unchanged when it fits.

`tests/automated/check-geoutils-debug-bounds.py` protects these paths.
