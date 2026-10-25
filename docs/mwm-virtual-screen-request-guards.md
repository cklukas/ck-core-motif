---
nav_exclude: true
---

# MWM virtual-screen request bounds

The WSM PAN request contains two 32-bit coordinates and one byte; GOTO contains
two 32-bit coordinates. `WMiConvert` now checks the 8-bit protocol format and
minimum payload lengths before unpacking either request. Invalid requests are
ignored safely, while valid virtual-screen operations retain their protocol
behavior.

The focused regression check is
`tests/automated/check-mwm-virtual-screen-request-guards.py`.
