# Path normalization empty-path guard

The Motif path helper normalizes a constructed path by checking its final
character before appending a slash.  A valid empty result can reach that
normalization step, so indexing `userDirLen - 1` without a zero-length check
would read before the allocated string.  The helper now treats an empty path
as needing a slash, preserving the existing normalized-path contract without
an out-of-bounds read.

The regression contract is checked by
`tests/automated/check-xmos-empty-path.py`.
