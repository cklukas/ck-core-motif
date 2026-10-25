# PushButton label geometry

PushButton computes the label rectangle after accounting for highlight and
shadow thickness. Those values are stored in unsigned `XRectangle` dimensions,
so a button smaller than its decorations could previously wrap a negative
content size to a near-maximum rectangle. The dimensions now clamp to zero
when the decorations consume the available space; normal-sized buttons keep
the same geometry.
