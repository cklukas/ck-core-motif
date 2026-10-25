---
nav_exclude: true
---
# Minimized-icon placement sentinel guard

`ShowIconForMinimizedClient` now rejects a negative or oversized placement
returned by coordinate conversion before indexing the workspace placement list.
The client is left without a placement when no valid slot can be derived.

`tests/automated/check-show-icon-placement-sentinel.py` protects this contract.
