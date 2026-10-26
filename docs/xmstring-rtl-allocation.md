---
nav_exclude: true
---
# RTL compound-string scratch allocation

The RTL compound-string renderer now returns safely when its temporary buffer
allocation fails, instead of writing through a NULL pointer. Successful RTL
rendering and existing error returns remain unchanged.

The focused regression is
`tests/automated/check-xmstring-rtl-allocation.py`.
