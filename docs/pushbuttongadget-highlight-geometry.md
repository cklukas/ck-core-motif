# PushButtonGadget highlight geometry

The internal PushButtonGadget focus highlight subtracts the 3-D enhancement
pixel from the configured highlight thickness and then subtracts the resulting
inset from the gadget rectangle.  The path now performs the first subtraction
in a signed integer, rejects a non-positive highlight width, and skips drawing
when the inset consumes the gadget.  This prevents unsigned wraparound and
negative rectangles on small controls while preserving normal focus visuals.
