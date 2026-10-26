---
nav_exclude: true
---
# MWM border-metric input validation

`UpperBorderWidth` and `LowerBorderWidth` return zero for a null
`ClientData` pointer.  This avoids reading decoration, matte, or bevel state
from an absent client; valid border metrics are unchanged.

The source-contract regression is
`tests/automated/check-mwm-border-metrics-inputs.py`.
