---
nav_exclude: true
---
# ArrowButton gadget origin bounds

ArrowButton gadget redisplay computes its fill and shadow origins from X11
`Position` coordinates and unsigned highlight/shadow dimensions. C's usual
integer conversions can otherwise wrap a negative or near-edge origin before
it reaches the X drawing call.

`ArrowBG.c` now promotes each operand to a wide signed type before adding the
dimensions, then saturates the final origin and extent at the drawing
boundary. The matching ArrowButton widget path follows the same rule. This
preserves the Motif API and the existing drawing geometry, while keeping
narrow or near-edge controls from receiving a wrapped origin or extent.

The source regression `tests/automated/check-arrow-drawing-insets.py` keeps
both widget and gadget inset calculations on the bounded-arithmetic path.
