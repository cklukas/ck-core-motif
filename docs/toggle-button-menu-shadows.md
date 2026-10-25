---
nav_exclude: true
---
# ToggleButton menu shadow bounds

The ToggleButton menu enter and border-highlight callbacks now draw shadows
only when the widget has a positive interior after its highlight frame and both
shadow GCs are available.  Core dimensions are converted before subtraction,
preventing invalid rectangles for undersized or partially initialized menu
entries.

Normal-sized menu entries retain their existing shadow type and callback order.
The focused regression is
`tests/automated/check-toggle-button-menu-shadows.py`.
