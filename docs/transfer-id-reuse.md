---
nav_exclude: true
---
# Transfer context reuse

Transfer contexts are now initialized before they are published on the global
context chain. Recycled contexts also clear their done-procedure pointer and
count after freeing the old list, preventing concurrent or reused transfers
from observing stale callback state.

Regression coverage: `tests/automated/check-transfer-id-reuse.py`.
