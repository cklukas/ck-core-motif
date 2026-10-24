# Mrm buffer decommit state

`Idb__BM_Decommit` now rejects modified write buffers that have lost their file
owner, and clears activity/modified state after every successful decommit,
including read-only buffers.  Released buffers are therefore reusable without
silently retaining stale activity.

The source-level regression is `tests/automated/check-mrm-decommit-state.py`.
