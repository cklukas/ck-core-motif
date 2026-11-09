---
nav_exclude: true
---
# Spinner geometry negotiation

`XmSpinner` supports independent automatic dimensions.  During construction
and later `XtSetValues`, a zero width or height requests the content-derived
value for that axis.  A nonzero dimension remains an application constraint.
`QueryGeometry` now reflects the same contract, rather than always returning
the preferred size.
