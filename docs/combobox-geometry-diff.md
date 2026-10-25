# ComboBox geometry negotiation bounds

When the scrolled list replies with `XtGeometryAlmost`, ComboBox now computes
the height difference in signed arithmetic and clamps the resulting edit-box
height.  A reply larger than the available edit area cannot wrap the next
geometry request to a huge unsigned height.

The source-level regression is
`tests/automated/check-combobox-geometry-diff.py`.
