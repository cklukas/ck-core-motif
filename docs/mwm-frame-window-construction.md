# MWM frame-window construction failure handling

`ConstructFrame` builds the client frame, optional title window, and base
window in that order. X11 can return `None` when a server cannot allocate a
window. The constructor now checks every allocation before using the resource,
and destroys the partial frame hierarchy when a later allocation fails.

This keeps resource-exhaustion failures local: MWM returns `False` to its
caller instead of issuing child-creation, mapping, or drawing requests against
an invalid window ID.

The source guard is covered by
`tests/automated/check-mwm-frame-window-construction.py`.
