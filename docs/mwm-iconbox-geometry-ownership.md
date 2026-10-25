---
nav_exclude: true
---

# MWM icon-box geometry ownership

Workspace resource processing needs an owned copy of `iconBoxGeometry` because
the resource value may be released during workspace renaming. The processing
path now makes exactly one copy before later ownership transfer; a redundant
second copy that overwrote the first and leaked it has been removed.

Resource values and workspace geometry behavior are unchanged. The focused
regression check is
`tests/automated/check-mwm-iconbox-duplication.py`.
