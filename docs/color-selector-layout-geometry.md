# ColorSelector layout geometry

`XmColorSelector` derives child widths and heights by subtracting margins,
radio-box geometry, and borders from the selector's current size. Those
members are X toolkit unsigned dimensions, so doing the subtraction directly
could wrap a tiny selector into a nearly 64K-wide child. The layout now uses
signed intermediates, clamps negative available space to zero, and avoids
subtracting borders from a color area that is too small to contain them. The
radio-box centering calculation also performs its subtraction in signed
arithmetic.

This preserves the normal layout for usable sizes while making very small or
temporarily collapsed selectors deterministic during resize and realization.
