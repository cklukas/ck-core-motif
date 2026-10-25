# MWM TARGETS conversion allocation guard

MWM’s selection conversion now checks its fixed TARGETS atom-array allocation
before writing protocol atoms. Allocation failure returns a failed conversion
with the already-initialized empty output fields.

The focused regression is
`tests/automated/check-mwm-targets-conversion-allocation.py`.
