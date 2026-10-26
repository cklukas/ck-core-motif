---
nav_exclude: true
---
# MWM frame/client conversion input validation

`FrameToClient` and `ClientToFrame` are legacy `void` helpers, so malformed
calls cannot report an error.  They now return without modifying any argument
when the `ClientData` record or one of the four coordinate/size pointers is
null.  Existing valid calls use the same conversion arithmetic.

The source-contract regression is
`tests/automated/check-mwm-frame-client-conversion-inputs.py`.
