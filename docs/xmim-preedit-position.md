# Input-method preedit-position bounds

The XIM preedit-position path now computes focused-widget space in signed
arithmetic and clamps it at zero before sizing the preedit rectangle.  Narrow
widgets with large primitive margins no longer produce wrapped unsigned
widths or heights.

The source-level regression is
`tests/automated/check-xmim-preedit-position-bounds.py`.
