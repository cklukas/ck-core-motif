# ComboBox hit and drawing bounds

ComboBox hit-area, arrow, and shadow rendering now clamp available dimensions
after insets to at least one pixel.  Collapsed controls no longer pass wrapped
unsigned heights or shadow spans to hit testing and drawing code.

The source-level regression is
`tests/automated/check-combobox-drawing-bounds.py`.
