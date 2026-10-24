# Xmos mask-name construction

`_XmOSGenerateMaskName` derives a pixmap mask name by copying the measured
prefix of an image path, then inserting `_m` before its suffix.  The old
`strncpy` call intentionally copied a non-terminated prefix and relied on the
following writes, which triggered compiler truncation diagnostics and made the
copy contract unclear.

The prefix is now copied with exact-length `memcpy`; the existing suffix and
terminator writes remain unchanged.  Normal mask names and caller-owned
storage requirements are preserved.

`tests/automated/check-xmos-mask-name.py` protects the exact-copy contract.
