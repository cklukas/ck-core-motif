---
nav_exclude: true
---

# SegmentedControl child-key guards

SegmentedControl validates that child-change and child-key callback closures
refer to the expected widget class before reading private state. Foreign or
stale closures are ignored safely while normal selection and keyboard
navigation remain unchanged.
