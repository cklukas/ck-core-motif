---
nav_exclude: true
---
# Container conversion refusal input validation

The private Container refusal callback now returns safely when the conversion
callback payload is absent, instead of writing through a null pointer. Normal
conversion refusal still sets the existing refusal status and empty result.

The source contract is checked by
`tests/automated/check-container-convert-refuse-input.py`.
