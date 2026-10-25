---
nav_exclude: true
---
# Icon-place search guards

`FindIconPlace` now rejects missing clients, placement storage, and invalid
cell dimensions before searching or extending the icon grid.  Root-window
position conversion also returns `(0, 0)` when coordinate arrays are absent.

`tests/automated/check-find-place-guards.py` protects these paths.
