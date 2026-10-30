---
nav_exclude: true
---
# List sensitivity cancellation

`XmList` can schedule delayed drag initiation and edge auto-scroll callbacks.
Changing a list to insensitive must cancel both timers and clear the transient
button/append state; otherwise a queued callback could select an item or start
a drag after the application disabled the widget.  The timeout callbacks also
check sensitivity defensively before doing any work.

The focused regression check is
`tests/automated/check-list-sensitivity-cancels-gestures.py`.
