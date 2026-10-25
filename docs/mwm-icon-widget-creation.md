---
nav_exclude: true
---
# MWM icon-box widget creation

MWM now checks icon-box item widget creation before applying the placeholder
label, callbacks, or window-dependent state. If allocation fails, icon-box
initialization returns through its existing failure path instead of using a
null child.

`tests/automated/check-mwm-icon-widget-creation.py` protects this path.
