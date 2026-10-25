---
nav_exclude: true
---
# Mwm single-screen navigation index

`F_Screen` keeps the previous screen for the `back` action. With one managed
screen, `wmGD.Screens` is an ordinal array whose only valid index is zero,
while Xlib may report a nonzero default screen number. The initialization now
maps that single-screen case to index zero; multi-screen displays retain the
existing screen-number behavior.

`tests/automated/check-mwm-fscreen-default.py` guards this mapping.
