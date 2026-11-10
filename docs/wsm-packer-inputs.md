---
nav_exclude: true
---

# WSM packer input validation

The internal WSM request and reply packers initialize any supplied length and
error outputs before validating their required inputs.  A missing request,
reply, length output, error output, or request display now returns no message
without dereferencing the incomplete caller state.

Regression: `tests/automated/check-wsm-packer-inputs.py`.
