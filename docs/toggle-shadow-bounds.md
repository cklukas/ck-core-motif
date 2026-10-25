# ToggleButton gadget shadow bounds

ToggleButton gadget menu-arming and redisplay paths now check that the gadget
is wider and taller than twice its highlight thickness before drawing a shadow.
Collapsed gadgets therefore skip the shadow instead of converting a negative
inset span to a huge unsigned rectangle.

The source-level regression is `tests/automated/check-toggle-shadow-bounds.py`.
