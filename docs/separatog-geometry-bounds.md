---
nav_exclude: true
---
# Separator gadget geometry bounds

Separator gadget redisplay now clamps the inset fill and separator dimensions
to zero when the gadget is narrower or shorter than its highlight insets.
Collapsed gadgets skip those drawing calls instead of converting negative
spans to large unsigned rectangles.

The source-level regression is `tests/automated/check-separatog-geometry-bounds.py`.
