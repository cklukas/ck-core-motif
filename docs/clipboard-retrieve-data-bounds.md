# Clipboard retrieve data bounds

`ClipboardRetrieve` now validates format-data retrieval status and pointers,
rejects copied offsets beyond the available data instead of allowing unsigned
underflow, and handles empty foreign data without invalid pointer arithmetic.
Zero-length results skip `memcpy`; valid retrieval and incremental-copy behavior
remain unchanged.
