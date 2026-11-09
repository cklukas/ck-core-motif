---
nav_exclude: true
---
# Virtual-binding X11 property length

Fallback virtual-key bindings may originate in a user binding file, whereas
`XChangeProperty` accepts its element count as `int`. The binding publisher now
validates the byte length before that conversion. An oversized binding remains
available to the current process but is not published with a truncated root
window property length.

The focused regression check is
`tests/automated/check-virtual-binding-property-length.py`.
