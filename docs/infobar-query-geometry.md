---
nav_exclude: true
---
# InfoBar geometry negotiation

`XmInfoBar` now reports its current width and height through the normal Xt
geometry-query contract.  A parent layout manager therefore sees application
sizes such as `XmNwidth` and `XmNheight`, rather than the widget's original
360x34 defaults after those resources have been changed.

`tests/automated/check-info-bar.c` exercises an explicit 360x40 InfoBar through
`XtQueryGeometry`; `tests/automated/check-infobar-query-geometry.py` preserves
the source-level query contract.
