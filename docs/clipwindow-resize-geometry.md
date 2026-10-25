---
nav_exclude: true
---
# ClipWindow RTL resize geometry

When a right-to-left `XmClipWindow` is resized, each managed child keeps its
distance from the right edge.  The old implementation subtracted a signed
child `Position` from an unsigned widget `Dimension`; a child with a negative
or otherwise out-of-range position could therefore wrap the distance and move
to an unrelated location.

The resize path now performs the calculation in signed integers and clamps the
result to the X11 `Position` range before configuring the child.  Normal RTL
resizes retain their existing coordinates while transient or clipped child
positions remain deterministic.
