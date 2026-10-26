---
nav_exclude: true
---
# Compound-text zero-length append

The internal compound-text encoder accepts a zero-length append without a
source buffer.  Its concatenation helper now skips `memcpy` for that case,
avoiding an invalid NULL pointer operation while preserving the resulting
terminator and all non-empty appends.

`tests/automated/check-ctext-concat-null.py` protects this contract.
