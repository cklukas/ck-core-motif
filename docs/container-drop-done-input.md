---
nav_exclude: true
---
# Container drop-finish callback input validation

The private drop-finish callback now returns safely when no Container payload
was supplied, instead of dereferencing a null client pointer. Valid drag
completion still clears the Container's drag context.

The source contract is checked by
`tests/automated/check-container-drop-done-input.py`.
