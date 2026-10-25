---
nav_exclude: true
---

# MWM executable identity basename

MWM distinguishes `mwm` from `dtwm` using the executable name. Path-based
invocations now copy the basename after the final slash, so `/usr/bin/mwm`
selects the same identity as `mwm`; the previous copy retained a leading slash
and missed that match.

The focused regression check is
`tests/automated/check-mwm-identity-basename.py`.
