---
nav_exclude: true
---

# Spinner label centering

Spinner now computes the label's vertical offset in signed arithmetic. A label
taller than the widget remains centered with a negative offset rather than
wrapping the unsigned `Dimension` subtraction to a distant position.
