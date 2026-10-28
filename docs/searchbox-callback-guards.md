---
nav_exclude: true
---

# SearchBox callback guards

SearchBox validates its widget type before query and commit callback delivery,
and validates event data before clear-button or Escape handling. Invalid
teardown or synthetic dispatch is ignored safely.
