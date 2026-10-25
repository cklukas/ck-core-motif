---
nav_exclude: true
---
# WSM per-match attribute allocation failure

During `WM_WINDOWS` request unpacking, each match's combined allowed and
prohibited attribute block is now checked before use. If a later block fails,
already allocated blocks and the pointer array are released, the count is
cleared, and unpacking returns without partial state.

`tests/automated/check-wsm-attr-pair-oom.py` protects this rollback path.
