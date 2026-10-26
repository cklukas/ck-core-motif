---
nav_exclude: true
---

# Transfer-context API inputs

Transfer completion, parameter, value, and MULTIPLE-request APIs now reject a
null transfer context before dereferencing it or looking up its application
context. Invalid callback handles are no-ops; valid transfer queue ownership
and cleanup behavior is unchanged.

The focused regression check is
`tests/automated/check-transfer-context-inputs.py`.
