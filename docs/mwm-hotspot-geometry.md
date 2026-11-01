# MWM hotspot geometry

MWM uses an X11 `XRectangle` for system-menu hotspot hit testing.  Its origin
coordinates are signed 16-bit values, while its width and height are unsigned
16-bit values.  The hit test now widens both coordinates and extents before
forming the exclusive right and bottom edges.

This preserves ordinary hotspot behavior and fixes large (at least 32768-pixel)
hotspots, whose extents previously became negative through a signed `short`
conversion and could fail to receive pointer events.

`tests/automated/check-mwm-hotspot-geometry.py` verifies that the test keeps
the unsigned extent values intact.
