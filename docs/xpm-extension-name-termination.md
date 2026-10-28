---
nav_exclude: true
---

# XPM extension-name termination

Parsed `XPMEXT` names are now length-validated, allocated with space for the
terminating NUL, and explicitly terminated. Malformed records with an invalid
name span are rejected before allocation.

Regression: `tests/automated/check-xpm-extension-name-termination.py`
