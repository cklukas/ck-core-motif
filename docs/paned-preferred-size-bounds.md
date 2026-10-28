---
nav_exclude: true
---
# XmPaned preferred-size bounds

`XmPaned` computes its preferred major-axis size by adding pane dimensions,
borders, sash spacing, and margins. Those values are `Dimension`-sized, so
performing each addition directly could wrap a large layout before the final
window-size calculation.

The accumulation now saturates every addition at the maximum X11
`Dimension`. Normal layouts retain their existing geometry; oversized layouts
remain bounded instead of becoming a small or otherwise invalid request.

The focused source regression is
`tests/automated/check-paned-preferred-size-bounds.py`.
