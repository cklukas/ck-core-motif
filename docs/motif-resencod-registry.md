---
nav_exclude: true
---
# Segment-encoding registry allocation

Segment-encoding registration now allocates replacement strings before changing
an existing entry, and rolls back partial allocations for new entries.  Registry
export checks its output allocation before writing and resets the reported
length on failure.

`tests/automated/check-resencod-registry-allocation.py` protects these paths.
