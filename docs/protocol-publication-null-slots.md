---
nav_exclude: true
---

# Protocol property publication

Protocol property updates now skip null manager and protocol slots and safely
return when a nonempty manager has no backing protocol array. Valid active
protocols are published exactly as before.

The focused regression check is
`tests/automated/check-protocol-publication-null-slots.py`.
