---
nav_exclude: true
---
# ProgressRing graphics-context setup

ProgressRing now releases any graphics contexts already created when one of its
three GC allocations fails, then resets all GC fields. Expose and destroy paths
can therefore handle partial server-resource setup without stale handles.

The focused source contract is checked by
`tests/automated/check-progress-ring-gc-setup.py`.
