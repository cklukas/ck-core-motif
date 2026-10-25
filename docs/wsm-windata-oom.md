---
nav_exclude: true
---
# WSM window-data unpack allocation failure

The shared WSM window-data unpacker now initializes its output pointers,
rejects missing configuration metadata, checks the bit-mask allocation, and
checks the resulting data-record allocation. Failure clears the output count
and releases temporary storage before returning, so callers can clean up
safely.

`tests/automated/check-wsm-windata-oom.py` protects these contracts.
