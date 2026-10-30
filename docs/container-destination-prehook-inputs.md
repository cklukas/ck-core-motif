---
nav_exclude: true
---
# Container destination prehook input validation

The Container destination prehook now rejects a null widget or destination
callback structure before reading selection or transfer fields. This keeps
malformed callback delivery from dereferencing invalid input while preserving
normal drag-and-drop transfers.

The source contract is checked by
`tests/automated/check-container-destination-prehook-inputs.py`.
