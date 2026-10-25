# ComboBox collapsed geometry

ComboBox now clamps the dropdown shell width to one pixel when the control is
smaller than its highlight insets.  Its right-side arrow hit-area x coordinate
also uses signed arithmetic, so narrow controls do not wrap popup or hit-test
geometry to a distant positive position.

The source-level regression is
`tests/automated/check-combobox-collapsed-geometry.py`.
