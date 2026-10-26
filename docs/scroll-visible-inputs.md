---
nav_exclude: true
---
# Scroll-visible input guards

`XmScrollVisible` now returns immediately when either the scrolled-window
widget or the descendant widget is null. The validation happens before the
widget cast and application-context lookup, preventing invalid traversal
requests from crashing the caller. Existing warning behavior for non-null
widgets that are not suitable scrolled windows is unchanged.

The focused regression is
`tests/automated/check-scroll-visible-inputs.py`.
