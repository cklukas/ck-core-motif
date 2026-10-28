---
nav_exclude: true
---

# Toast callback guards

Toast action and timeout callbacks validate their widget data before reading
private state. Foreign, stale, or teardown-time callback data is ignored
safely while normal action and timeout dismissal remain unchanged.
