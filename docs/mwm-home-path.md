---
nav_exclude: true
---

# MWM home-directory path bounds

The legacy home-directory resolver now formats `$HOME` and passwd-derived
paths into its fixed `MAXWMPATH` buffer with bounded output. Overlong
environment or passwd values can no longer overwrite adjacent MWM state.
