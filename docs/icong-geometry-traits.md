---
nav_exclude: true
---

# IconGadget geometry-trait validation

`XmIconGadget`'s private baseline, display-rectangle, and margin traits now
reject missing widget or output arguments before accessing gadget state.  The
display rectangle also reports zero width or height when a caller constrains an
IconGadget below twice its highlight thickness, rather than wrapping the
unsigned `Dimension` subtraction to a large rectangle.

Valid geometry and trait behavior is unchanged.  The focused source-level
regression is `tests/automated/check-icong-geometry-traits.py`.
