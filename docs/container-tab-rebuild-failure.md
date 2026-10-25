---
nav_exclude: true
---
# Container dynamic tab-list rebuild failures

`XmContainer` now builds a replacement dynamic detail tab list before releasing
the current one. If tab allocation fails, the current list remains available so
the container can keep its last usable column layout. Geometry that is not yet
valid still returns no dynamic list as before.
