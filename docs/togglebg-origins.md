---
nav_exclude: true
---
# ToggleButton gadget draw origins

ToggleButton gadget shadow and highlight calls now add highlight thickness to
`Position` coordinates in signed arithmetic.  Gadgets positioned partly
outside a parent therefore retain their negative/off-edge origin instead of
wrapping to a distant positive coordinate.

The source-level regression is `tests/automated/check-togglebg-origins.py`.
