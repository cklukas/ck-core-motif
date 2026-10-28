---
nav_exclude: true
---

# RatingControl event guards

RatingControl now validates its widget type and event pointer before handling
pointer or keyboard input. This makes teardown and synthetic callback
dispatch safe without changing live rating interactions.
