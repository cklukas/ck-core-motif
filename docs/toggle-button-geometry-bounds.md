# ToggleButton geometry bounds

ToggleButton drawing uses dimensions supplied by the Xt geometry contract.
Those dimensions are unsigned, so subtracting highlight and shadow margins
before checking the result can wrap a very small widget into a large fill or
shadow rectangle.  The shadow path also must not ask Xlib to draw with missing
shadow graphics contexts.

`DrawToggleShadow`, `DrawToggleLabel`, and `DrawEtchedInMenu` now validate the
available width and height before subtracting their margins.  They return
without issuing a drawing request when the interior is empty or when the
shadow GCs are unavailable.  Ordinary widgets with a positive interior keep
the existing geometry and visual behavior.

The source contract is covered by
`tests/automated/check-toggle-button-geometry-bounds.py`; the installed
library fixture `tests/automated/run-toggle-button-geometry.py` realizes
1×1 widgets through the public API under Xvfb and rejects any X protocol
error.  The full installed library build and gallery integration remain the
broader runtime qualification for the changed `libXm` object.
