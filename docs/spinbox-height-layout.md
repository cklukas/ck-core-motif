---
nav_exclude: true
---

# SpinBox constrained-height layout

When a SpinBox is shorter than its preferred height, vertical arrow spacing is
computed from the available height.  The calculation must not use the widget's
width; doing so made the arrows overflow or move when only the width changed.
The constrained-height regression covers classic, modern, and high-contrast
profiles, scale overrides, arrow layouts, and right-to-left direction.
