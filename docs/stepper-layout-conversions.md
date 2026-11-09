---
nav_exclude: true
---

# Stepper layout conversions

Stepper calculates geometry as signed working values after bounding the widget
allocation to the X11 coordinate range.  It now converts those proven values
explicitly at the Xt `Position` and `Dimension` boundary, preserving small
layout behavior without implicit signed/unsigned narrowing.

NumberBox applies the same rule to editor, arrow, and error-label allocation;
the error label cannot receive a negative origin on a very small allocation.
