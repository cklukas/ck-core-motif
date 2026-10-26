# MWM active icon-label lifecycle

The active icon label is an optional helper window. Its hide and move paths
now validate the selected screen/client and use a single resolved window ID.
This matters when allocation failed, a screen is being torn down, or cleanup
is requested without an explicit screen pointer: no context or X11 window
operation is attempted with `None`.
