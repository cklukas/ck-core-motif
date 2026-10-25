# Mrm compression-table input checks

The compression helpers are internal boundaries shared by widget creation and
resource-reference handling. They now reject null file, output, and return
pointers before dereferencing them, and reject table entries that do not contain
a string or class descriptor. Subtree checks return false for an unavailable or
invalid table. Valid compressed resource and class lookups keep their existing
results and diagnostics.

The source-level regression check is
`tests/automated/check-mrm-compression-inputs.py`.
