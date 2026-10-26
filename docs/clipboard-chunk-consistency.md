---
nav_exclude: true
---

# Keep clipboard chunks consistent

`GetWindowProperty` now records the first chunk's property type and format and
rejects later chunks that change either value.  This protects INCR reads from
an external owner replacing the selection mid-transfer and prevents mixed
format data from being exposed to callers.
