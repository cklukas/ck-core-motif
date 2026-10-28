# I18List cell right-edge bounds

`MakeCellVisible` now computes a cell's right edge in a wide type and clamps
it before passing the value to the legacy visibility helper.  This prevents a
wide column plus its starting offset from wrapping through `short` and
making the wrong portion of the cell visible.

The source-contract regression is
`tests/automated/check-i18-list-cell-edge-bounds.py`.
