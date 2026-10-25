---
nav_exclude: true
---

# Desktop color-cell allocation failures

`XmeGetDesktopColorCells` builds a list of usable desktop colors, including
standard icon colors. The icon-color path now checks both `XParseColor` and
`XAllocColor` before publishing a result. If a display cannot parse or allocate
one of those colors, that entry is skipped instead of exposing an uninitialized
pixel. Existing successful allocations and the optional result-count behavior
are unchanged.

The focused regression check is
`tests/automated/check-desktop-color-cells-allocation.py`.
