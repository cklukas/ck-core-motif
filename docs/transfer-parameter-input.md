---
nav_exclude: true
---

# Transfer parameter input validation

`XmTransferSetParameters` now rejects a nonzero parameter length when the
parameter buffer is `NULL`, before forwarding the request to Xt. Zero-length
parameters and non-null buffers retain their existing format and atom
validation behavior.

The focused source-contract check is
`tests/automated/check-transfer-context-inputs.py`.
