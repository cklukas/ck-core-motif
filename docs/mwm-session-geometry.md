---
nav_exclude: true
---

# MWM session geometry persistence

MWM persists restored client geometry with signed `%d` resource values. The
session writer now retains signed dimensions, clamps a client size at or below
its base size to zero increments, and treats a non-positive increment as an
unscaled positive client size. This prevents malformed or transient size hints
from becoming a huge unsigned value in the saved session database while
preserving valid ICCCM geometry calculations.
