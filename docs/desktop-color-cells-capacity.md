---
nav_exclude: true
---

# Desktop color-cell capacity validation

`XmeGetDesktopColorCells` now rejects non-positive `n_colors` values before
performing color-object or X-server work. Positive capacities and the existing
optional result-count behavior remain unchanged.

The focused regression check is
`tests/automated/check-desktop-color-cells-capacity.py`.
