---
nav_exclude: true
---
# Container anchor-toggle constraint guards

When extended selection toggles the anchor item, Container now returns safely
if the anchor has no private constraint record before reading its selection
state.  Valid anchor toggles and marquee setup remain unchanged.

The source contract is checked by
`tests/automated/check-container-anchor-toggle-constraint-guards.py`.
