---
nav_exclude: true
---

# MWM X-error format allocation

In DEBUG builds, MWM copies Xlib error-format strings returned into a caller
buffer by `XGetErrorDatabaseText`. The copy now checks the measured length
against Xt's `Cardinal` width before allocation. The existing fatal
initialization path remains unchanged for an unrepresentable length or
allocation failure.
