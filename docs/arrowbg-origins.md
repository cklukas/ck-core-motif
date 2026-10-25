# ArrowButton gadget origin bounds

ArrowButton gadget redisplay computes its fill and shadow origins from X11
`Position` coordinates and unsigned highlight/shadow dimensions. C's usual
integer conversions can otherwise wrap a negative or near-edge origin before
it reaches the X drawing call.

`ArrowBG.c` now promotes each operand to `int` before adding the dimensions,
then converts the bounded shadow origin back to `Position` at the drawing
boundary. This preserves the Motif API and the existing drawing geometry,
while keeping narrow or near-edge gadgets from receiving a wrapped origin.

The source regression `tests/automated/check-arrowbg-origins.py` keeps both
the fill and shadow calculations on the signed-arithmetic path.
