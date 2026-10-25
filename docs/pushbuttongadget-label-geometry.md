# PushButtonGadget label geometry

PushButtonGadget stores its label rectangle in the shared integer
`LRectangle`, but the source rectangle dimensions are unsigned. When the
highlight and shadow decorations consume a tiny gadget, direct subtraction
could produce a negative rectangle that later drawing code interpreted as a
large region. The computed label width and height now clamp to zero while
normal gadget geometry remains unchanged.
