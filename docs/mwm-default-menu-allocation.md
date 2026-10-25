---
nav_exclude: true
---
# MWM default-menu allocation

MWM assembles the localized default system menu before copying it into the
resource string used by the menu parser.  If that temporary assembly buffer
cannot be allocated, initialization now warns and falls back to the built-in
system-menu definition instead of formatting through null storage.  Successful
localized menus are unchanged.

`tests/automated/check-mwm-default-menu-allocation.py` protects this contract.
