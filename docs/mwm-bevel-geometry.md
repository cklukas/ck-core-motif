# MWM bevel primitive geometry validation

The shared `BevelRectangle()` and `BevelDepressedRectangle()` primitives now
validate their inputs before doing unsigned geometry arithmetic.  They return
without drawing when either rectangle list is missing, the box is empty, bevel
widths cannot fit, width sums could overflow, or a depressed inset is wider
than one of its four bevel sides.

Valid callers retain the existing shadow-list output.  This protects all MWM
frame and gadget drawing paths from malformed dimensions without changing the
public Motif interfaces.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-bevel-geometry.py
```
