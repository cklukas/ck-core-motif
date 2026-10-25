# Mrm write-open output initialization

`UrmIdbOpenFileWrite` now clears `file_id_return` before opening the low-level
file or allocating the descriptor. A failed create, allocation, or header
initialization therefore cannot leave a stale handle in the caller’s output
slot. Successful write opens retain their existing behavior.
