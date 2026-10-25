---
nav_exclude: true
---
# Mwm include-stack allocation warnings

When an included resource file cannot be added to the WSM configuration stack,
`mwm` reports the included filename.  Both allocation-failure branches used
the shared `wmGD.tmpBuffer`, so long filenames or translations could overflow
that fixed buffer.

A shared warning helper now measures the complete message, checks that the
formatter result fits Xt's `Cardinal` width, and allocates it for the duration
of `Warning`. Both stack initialization and push paths retain their existing
recovery behavior.

`tests/automated/check-mwm-config-allocation-warning.py` protects this contract.
