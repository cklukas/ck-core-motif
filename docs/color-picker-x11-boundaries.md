---
nav_exclude: true
---

# ColorPicker X11 boundaries

The ColorPicker keeps layout arithmetic in signed working values and now makes
the final X11 boundary explicit.  Empty spans stay empty, positive rectangle
extents are bounded before unsigned X requests, and invalid drawable depths
are rejected before pixmap or image construction.

For indexed visuals, an empty or invalid visual map no longer becomes a
negative XQueryColors count.  Palette allocation results are normalized to
Motif `Boolean` values, and the bounded fallback map is queried only when it
contains entries.  HSV conversion also tests its floating-point maximum
explicitly before division.

The result is unchanged normal rendering with safe behavior for constrained
widgets, malformed visual metadata, and unusually large resource values.
