---
nav_exclude: true
---

# List horizontal scroll range

List now clamps its horizontal scrollbar maximum, slider extent, and origin
as a single range whenever content, padding, or font metrics change. A stale
origin can no longer make the slider size zero or leave the value beyond the
new maximum after a shrink.
