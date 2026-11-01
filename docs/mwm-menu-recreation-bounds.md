---
nav_exclude: true
---
# MWM menu-widget recreation bounds

When a client command changes, MWM collects affected menu specifications,
destroys their old widgets, and recreates them. The temporary list now tracks
its count in `size_t`, rejects an impossible count before it can wrap, and
checks native and Xt `Cardinal` allocation widths. Valid menu recreation order
and callback behavior are unchanged.

`tests/automated/check-mwm-menu-recreation-bounds.py` protects this boundary.
