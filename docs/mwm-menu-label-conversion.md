---
nav_exclude: true
---
# MWM menu label conversion

Menu construction now checks localized label and accelerator-text conversion
before passing results to Motif widgets. Failed conversions omit only the
affected presentation string while preserving the menu item and accelerator
action; successful labels, accelerators, and menu behavior are unchanged.

`tests/automated/check-mwm-menu-label-conversion.py` protects this path.
