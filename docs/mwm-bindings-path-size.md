---
nav_exclude: true
---

# MWM motif-bindings path sizing

MWM derives the user binding-file path from the home-directory name and the
`.motifbind` suffix. The combined size is now checked before conversion to
Xt's allocation type, so an unrepresentable path is rejected without a wrapped
allocation or an out-of-bounds `snprintf` destination.

Normal home-directory paths and binding lookup behavior are unchanged. The
focused regression check is
`tests/automated/check-mwm-bindings-path-size.py`.
