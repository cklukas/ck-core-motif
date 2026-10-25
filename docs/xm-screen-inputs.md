---
nav_exclude: true
---
# XmGetXmScreen state validation

`XmGetXmScreen` now returns cleanly when the display reports children without a
child array, or when the supplied `Screen*` is not one of that display’s
screens. Valid lookups and implied screen creation remain unchanged.

`tests/automated/check-xm-screen-inputs.py` protects this contract.
