---
nav_exclude: true
---
# Container conversion callback input validation

The Container selection conversion callback now rejects a null widget or
conversion callback structure before resolving atoms or reading target data.
Malformed callback delivery therefore returns safely without changing normal
selection conversion behavior.

The source contract is checked by
`tests/automated/check-container-convert-inputs.py`.
