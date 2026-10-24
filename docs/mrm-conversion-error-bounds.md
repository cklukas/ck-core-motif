# Mrm conversion diagnostic bounds

Widget creation, value conversion, callback, color, and resource lookup paths
format diagnostics into fixed 300-byte Mrm buffers. All 37 formatting sites in
`Mrmwcrw.c` now use `snprintf`, preserving normal short messages while safely
truncating long resource names, font strings, and other input-derived text.

The source-level regression is
`tests/automated/check-mrm-conversion-error-bounds.py`.
