---
nav_exclude: true
---
# MainWindow live layout

Changing a realized `XmMainWindow` margin or separator resource can require a
new child layout without changing the outer widget size. Xt does not call a
widget resize method for that unchanged-size case. MainWindow now explicitly
relayouts its children there, while retaining the existing geometry path when
the requested outer size changes.

The focused regression is
`tests/automated/check-mainwindow-width-query-bounds.py`.
