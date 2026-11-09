---
nav_exclude: true
---
# Mrm root-record allocation checks

Root leaf and node initialization now verifies that a successful buffer
allocation includes both a buffer object and record storage before initializing
the header.  Incomplete allocator results now return failure instead of
crashing during root creation.

The returned buffer is cleared before construction begins.  Root-node creation
does not publish its buffer until both child-parent links have been updated, so
callers cannot mistake a partially initialized record for a successful result.

The source-level regression is
`tests/automated/check-mrm-root-allocation.py`.
