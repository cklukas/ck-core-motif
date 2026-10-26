# Validate ICC stream region inputs

The ICC drop-site stream writer now validates its region before writing the
header.  Null regions, non-empty regions without rectangle storage, negative
rectangle counts, and counts that cannot be represented by the protocol's
32-bit field are refused without dereferencing invalid memory or emitting a
truncated stream.

Valid regions retain the existing wire format.  The focused source regression
is `tests/automated/check-drag-stream-region-input.py`.
