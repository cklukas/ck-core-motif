# Segment-encoding registry allocation

Segment-encoding registration now allocates replacement strings before
changing an existing entry, and rolls back partial allocations for new
entries. Registry export checks its output allocation before writing and
resets the reported length on failure.

The regression check is
`tests/automated/check-resencod-registry-allocation.py`.
