---
nav_exclude: true
---

# Label gadget display-rectangle validation

`XmLabelGadgetGetDisplayRect` now rejects a null widget or output rectangle
before reading geometry. Valid display rectangles are returned unchanged,
while malformed geometry queries no longer crash.

The focused regression check is
`tests/automated/check-label-display-rect-outputs.py`.
