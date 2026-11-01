---
nav_exclude: true
---

# MWM double-click time validation

`doubleClickTime` is an integer resource while X11 event timestamps are
unsigned. MWM now requires a positive configured threshold before comparing it
with an event interval. A zero or negative value therefore disables
double-click recognition instead of being converted to a huge unsigned timeout;
positive values retain the existing millisecond behavior.
