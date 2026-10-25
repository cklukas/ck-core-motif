---
nav_exclude: true
---
# ToggleButton indicator fallback

ToggleButton and ToggleButtonGadget now tolerate an unnormalized
`XmONE_OF_MANY` indicator state by skipping that draw operation. The normalized
diamond, round, and other documented indicator styles are unchanged.

The regression check is
`tests/automated/check-togglebutton-indicator-fallback.py`.
