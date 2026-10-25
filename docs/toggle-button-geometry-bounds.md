# ToggleButton geometry bounds

The ToggleButton shadow and fill paths now check the widget dimensions before
subtracting highlight and shadow margins.  Core dimensions are unsigned
`Dimension` values, so subtracting margins first could wrap a tiny widget into
a large positive rectangle after conversion to `int`.  Undersized widgets now
skip that draw operation, and the shadow path also skips calls with missing
shadow GCs.

Normal-sized toggles keep their existing margins, colors, and shadow ordering.
The focused regression is
`tests/automated/check-toggle-button-geometry-bounds.py`.
