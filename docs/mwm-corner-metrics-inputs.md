---
nav_exclude: true
---
# MWM corner-metric input validation

`CornerWidth` and `CornerHeight` return zero for a null client record.  Their
three-corner proportionality checks use a wide intermediate so large unsigned
metric values cannot wrap during comparison.  Valid metric results are
unchanged.

The source-contract regression is
`tests/automated/check-mwm-corner-metrics-inputs.py`.
