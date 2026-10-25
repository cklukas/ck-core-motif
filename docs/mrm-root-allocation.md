---
nav_exclude: true
---
# Mrm root-record allocation checks

Root leaf and node initialization now verifies that a successful buffer
allocation includes both a buffer object and record storage before initializing
the header.  Incomplete allocator results now return failure instead of
crashing during root creation.

The source-level regression is
`tests/automated/check-mrm-root-allocation.py`.
