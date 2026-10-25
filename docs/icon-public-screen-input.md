---
nav_exclude: true
---
# XmGetIconFileName screen input guard

`XmGetIconFileName` now returns no filename when its screen argument is null,
before asking Xt for the display. Valid screen-based icon lookup is unchanged.

The focused regression is `tests/automated/check-icon-public-screen-input.py`.
