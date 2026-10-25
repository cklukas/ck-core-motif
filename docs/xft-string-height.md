# Xft compound-string line height

Xft-backed compound-string geometry now uses `XftFont.height` for the line
height while retaining ascent and descent for baseline calculations.  This
preserves font leading and avoids clipping or compressing labels whose Xft
metrics are taller than ascent plus descent.

Core-font and font-set geometry are unchanged, and the existing underline
adjustments still apply after the Xft height is selected.
