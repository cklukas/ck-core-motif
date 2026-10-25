---
nav_exclude: true
---
# Cascade gadget position geometry

`XmCascadeButtonGadget` centers its submenu indicator inside the menu entry.
The old calculation used unsigned widget dimensions throughout.  If a menu
entry was temporarily smaller than the indicator, margins, or highlight
insets, horizontal subtraction wrapped and vertical centering produced a huge
positive offset.

The position calculation now uses signed intermediates, treats a negative
available vertical span as zero, and clamps the resulting coordinates to the
X11 `Position` range before storing them.  Normal menu layouts retain their
existing coordinates while tiny entries remain drawable and stable during
resize and realization.
