# MWM gadget-rectangle inputs

Frame-decoration helpers now handle missing client records safely:

- `AllocateGadgetRectangles(NULL)` returns `False`.
- `ComputeGadgetRectangles(NULL)` returns without changing state.

Valid client records use the existing allocation and geometry behavior.
