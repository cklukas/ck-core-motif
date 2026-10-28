---
nav_exclude: true
---

# SegmentedControl child-key guards

SegmentedControl validates that a child-key callback closure refers to the
expected widget class before reading private navigation state. Foreign or
stale closures are ignored safely while normal keyboard navigation remains
unchanged.
