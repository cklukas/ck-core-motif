---
nav_exclude: true
---

# Flyout event guards

Flyout validates its callback closure and event pointer before processing
owner or popup events. Invalid teardown-time dispatch is ignored safely while
normal dismissal behavior remains unchanged.
