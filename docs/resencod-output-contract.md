---
nav_exclude: true
---

# Compound-text resource-converter output

The compound-text resource converter now initializes `XrmValue.addr` and
`XrmValue.size` before parsing. Empty input and allocation/conversion failures
therefore leave a deterministic empty output instead of exposing stale fields
from the caller's `XrmValue`.

The focused regression check is
`tests/automated/check-resencod-output-contract.py`.
