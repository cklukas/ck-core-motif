---
nav_exclude: true
---

# InfoBar event guards

InfoBar now validates its widget type and event pointer before handling close,
action, and Escape input. Teardown or synthetic dispatch is ignored safely;
normal live-bar interactions are unchanged.
