# IconBox cell-span bounds

IconBox hit testing divides pointer coordinates by the cell size plus its
margin.  Applications may set those `Dimension` resources to zero, and a
wrapped or zero sum previously made `GetCellFromXY` divide by zero.  The
conversion now computes the spans in an unsigned type and substitutes a
one-pixel span when a sum is zero, keeping pointer hit testing defined for
degenerate resource values.

`tests/automated/check-iconbox-cell-span.py` keeps the nonzero-span guard in
the coordinate conversion path.
