---
nav_exclude: true
---
# ScrolledWindow viewport bounds

Automatic and application-defined ScrolledWindow layouts now clamp the clip
and work-area origin and extents to the widget's actual allocation. This keeps
scrollable content and visible scrollbar geometry inside very small panes,
including one-pixel width or height allocations, while preserving normal
layout at larger sizes. Automatic scrollbars whose complete thickness cannot
coexist with a positive viewport are temporarily suppressed and restored when
the allocation grows.

The focused source-contract check is
`tests/automated/check-scrolled-viewport-bounds.py`. The installed live-scale
consumer remains `tests/automated/run-display-scale-scrolled.py`.
Navigator-fit policy is covered by
`tests/automated/check-scrolled-navigator-fit.py`.
