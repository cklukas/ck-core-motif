---
nav_exclude: true
---

# WSM packer input validation

The internal WSM request and reply packers initialize any supplied length and
error outputs before validating their required inputs.  A missing request,
reply, length output, error output, or request display now returns no message
without dereferencing the incomplete caller state.

All malformed request payload, count, pointer, size, and allocation failures
also report `WSM_ERROR_INTERNAL` instead of leaving the caller with a null
message and an erroneous `WSM_SUCCESS` status.  Protocol-state failures retain
their more specific configuration error codes.

The WSM list maximum is the full `CARD16` range (65,535).  Its expression casts
the complemented value back to `CARD16` before widening it, avoiding signed
integer promotion that would otherwise turn the limit into `-1`.

Window-data lists are also rejected at the public request and reply boundaries
when their count exceeds the WSM packer's bounded data-list domain.  This
prevents malformed data from reaching the lower-level size and packing helpers.

Regression: `tests/automated/check-wsm-packer-inputs.py`,
`tests/automated/check-wsm-request-counts.py`, and
`tests/automated/check-wsm-reply-counts.py`.
