---
nav_exclude: true
---
# BreadcrumbBar redisplay guards

`XmBreadcrumbBar` now handles a missing drawing GC, null separator or segment
labels, and narrow focus geometry without invalid Xlib calls or unsigned
underflow. Normal segment sizing and navigation remain unchanged.

The focused source contract is checked by
`tests/automated/check-breadcrumb-bar-redisplay-guards.py`.
