---
nav_exclude: true
---
# MWM WSM initialization declarations

WSM initialization directly configures the CDE backdrop search path and reads
per-screen session hints. It now includes and declares those APIs explicitly.
The inactive backdrop window is initialized with X11's `None` value, matching
its `Window` type. These are build-contract corrections; they do not change
session or workspace behavior.
