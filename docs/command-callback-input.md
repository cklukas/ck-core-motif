---
nav_exclude: true
---

# Command callback input

The Command widget's list and text callbacks now validate their callback and
client payloads before reading them. Invalid delivery is ignored, preserving
normal selection and value-change behavior while preventing teardown or
embedder errors from becoming null-pointer dereferences.

The focused source contract is checked by
`tests/automated/check-command-callback-input.py`.
