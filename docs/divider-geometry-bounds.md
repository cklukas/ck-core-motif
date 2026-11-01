---
nav_exclude: true
---
# Divider geometry bounds

`XmDivider` saturates both natural dimensions after measured text, spacing,
line thickness and scaled padding are combined. Initialization, live changes
and QueryGeometry share the same calculation, preventing Dimension wraparound
for large resource values. Explicit axes retain their application-owned sizes.

Drawing bounds line thickness to the cross-axis allocation and omits reversed
or empty segments when a label is wider than the available space. Text is
clipped to the widget. Native coverage in `run-divider-geometry.py` includes
maximum spacing/thickness, tiny allocations and exact labeled-line thickness;
`check-divider-label-width.py` additionally guards the saturated calculation.
