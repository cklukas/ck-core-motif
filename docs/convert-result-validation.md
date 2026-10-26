---
nav_exclude: true
---

# Shared conversion result validation

`_XmConvertHandler` is the common boundary used by selection, clipboard, and
drag conversion paths.  Before returning success it now requires an X
selection format of 8, 16, or 32 bits and rejects a nonzero result length with
no value buffer.  Invalid callback results become the same deterministic empty
failure used by other conversion errors, preventing malformed data from
reaching consumers.

The regression check is
`tests/automated/check-convert-result-validation.py`.
