---
nav_exclude: true
---

# DnD destination callback inputs

The Container, Text, and TextField destination callbacks now validate their
widget, callback structure, and transfer ID before touching Xt or X11 state.
This makes cancellation and widget-destruction callbacks safe even when the
toolkit receives an incomplete destination notification.

The regression check is
`tests/automated/check-dnd-destination-callback-inputs.py`.
