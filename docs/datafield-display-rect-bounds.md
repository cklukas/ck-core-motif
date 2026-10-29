# DataField display-rectangle bounds

The DataField display-rectangle callback now mirrors the hardened TextField
contract. Margin and inset arithmetic uses widened unsigned values, a missing
output rectangle is rejected, origins are saturated to X11's signed coordinate
range, and width or height becomes zero when the margins consume the widget.

This prevents tiny or transient allocations from returning wrapped display
rectangles to input-method and accessibility consumers.

The source contract is checked by:

```text
python3 tests/automated/check-datafield-display-rect-bounds.py
```
