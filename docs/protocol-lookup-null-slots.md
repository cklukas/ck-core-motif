---
nav_exclude: true
---

# Protocol lookup null slots

Protocol lookup now skips null entries in a partially initialized protocol
array. Valid entries continue to match normally, while malformed internal
state no longer causes a null dereference during lookup or dispatch.

The focused regression check is
`tests/automated/check-protocol-lookup-null-slots.py`.
