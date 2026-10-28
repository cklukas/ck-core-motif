---
nav_exclude: true
---

# ToggleSwitch coordinate bounds

ToggleSwitch clamps its label coordinates before passing them to Motif's
signed `Position` drawing arguments.  Large switch spacing or font metrics
therefore cannot wrap the label to an unrelated screen coordinate.

`tests/automated/check-toggle-switch-coordinate-bounds.py` protects this
drawing contract.
