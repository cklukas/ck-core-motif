---
nav_exclude: true
---
# PushButtonGadget default-shadow geometry

The gadget default-shadow erase paths subtracted twice the emphasis inset from
unsigned rectangle dimensions before checking whether the result was usable.
Tiny menu entries could therefore wrap to a large clear region, and the later
positive-width check could not detect the wrapped value.

Both erase paths now reject rectangles that cannot contain the inset before
subtraction. Normal default-button rendering is unchanged while collapsed
gadgets avoid oversized clear operations.
