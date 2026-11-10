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

Regression: `tests/automated/check-wsm-packer-inputs.py`.
