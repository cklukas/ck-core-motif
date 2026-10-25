---
nav_exclude: true
---
# PushButton gadget draw origins

PushButton gadget shadow and highlight calls now add highlight thickness to
`Position` coordinates in signed arithmetic.  Gadgets positioned partly
outside a parent therefore retain their negative/off-edge origin instead of
wrapping to a distant positive coordinate.

The source-level regression is `tests/automated/check-pushbg-origins.py`.
