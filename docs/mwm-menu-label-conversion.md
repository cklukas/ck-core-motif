---
nav_exclude: true
---
# MWM menu label conversion

Menu construction now checks localized label conversion before passing the
result to the Motif widget. If conversion fails, the menu item keeps the
toolkit’s default label rather than receiving a null `XmString`; successful
labels and menu behavior are unchanged.

`tests/automated/check-mwm-menu-label-conversion.py` protects this path.
