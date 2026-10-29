---
nav_exclude: true
---
# ScrolledWindow viewport bounds

Automatic and application-defined ScrolledWindow layouts now clamp the clip
and work-area origin and extents to the widget's actual allocation. This keeps
scrollable content and visible scrollbar geometry inside very small panes,
including one-pixel width or height allocations, while preserving normal
layout at larger sizes.

The focused source-contract check is
`tests/automated/check-scrolled-viewport-bounds.py`. The installed live-scale
consumer remains `tests/automated/run-display-scale-scrolled.py`.
