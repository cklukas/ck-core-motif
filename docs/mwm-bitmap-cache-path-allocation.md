---
nav_exclude: true
---

# MWM bitmap-cache path allocation

After resolving a bitmap path, MWM copies it into the per-screen bitmap cache.
That copy now checks the measured path length against Xt's `Cardinal` width
before allocating and uses the checked width explicitly. An unrepresentable
path follows the existing lookup-failure path; ordinary cache keys and bitmap
loading behavior are unchanged.
