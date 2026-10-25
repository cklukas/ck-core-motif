# Mrm memory-backed file cleanup

UID files opened from a caller-owned memory buffer do not have a low-level file
handle. `UrmIdbCloseFile` now skips the filesystem close for those descriptors,
returns success, and still releases the descriptor and compression tables.
Hierarchy cleanup now closes every file descriptor, including memory-backed
ones, so in-memory hierarchies do not leak their per-file state.
