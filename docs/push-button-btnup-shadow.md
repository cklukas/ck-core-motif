# PushButton tear-off release shadow bounds

The PushButton tear-off release path can redraw a depressed shadow after menu
focus and geometry have changed.  Its `BtnUp` shadow call now requires positive
interior dimensions and both shadow GCs, converting the core dimensions before
subtracting the highlight frame.

Normal tear-off release feedback keeps its existing shadow type and focus
behavior.  The focused regression is
`tests/automated/check-push-button-btnup-shadow.py`.
