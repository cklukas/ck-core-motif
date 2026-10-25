---
nav_exclude: true
---

# Screen drag-icon helper input validation

Operation, state, and source icon helpers now return no icon when given a
missing widget or unresolved screen object. Normal icon lookup and lazy
creation remain unchanged.

The focused regression check is
`tests/automated/check-screen-icon-inputs.py`.
