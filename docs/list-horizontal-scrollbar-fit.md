---
nav_exclude: true
---

# List horizontal-scrollbar fit

`XmList` decides whether an `XmAS_NEEDED` horizontal scrollbar is required
from the drawable width left after margins, highlight, and shadow thickness.
That width is signed because a transiently tiny allocation can leave no
positive viewport.  The decision now clamps only that comparison to zero
instead of converting a negative value to `Dimension`; long content therefore
continues to show its scrollbar rather than being mistaken for content that
fits.  The state snapshot and return value also track the horizontal scrollbar
itself, so a visibility change is reported to the surrounding ScrolledWindow.

The source contract is checked by
`tests/automated/check-list-horizontal-scrollbar-fit.py`.
