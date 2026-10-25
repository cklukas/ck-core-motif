---
nav_exclude: true
---
# DragICC write-buffer growth

Drag-and-drop ICC serialization appends data to either a stack-backed or
heap-backed byte buffer.  The old path added only 1000 bytes, allowed size
wraparound, and overwrote the active pointer before checking allocation
failure.

The writer now checks the required byte count, expands until the whole write
fits within the `Cardinal` capacity contract, and commits the replacement
buffer only after successful allocation.  Failed growth returns zero without
altering the existing buffer.
