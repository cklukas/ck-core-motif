---
nav_exclude: true
---
# MWM depressed-gadget geometry input validation

`GetDepressInfo` returns `FALSE` when its client record, geometry outputs, or
inversion-width output is null.  The guard runs before decoration state or
bevel metrics are read.  Existing valid gadget geometry and bevel calculations
are unchanged.

The source-contract regression is
`tests/automated/check-mwm-depressed-gadget-inputs.py`.
