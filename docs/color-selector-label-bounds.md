# Color selector label bounds

The color selector displays the selected color name together with its RGB
value. Color names can come from application resources or the X color database,
and the old `sprintf` calls wrote them into a fixed `BUFSIZ` label buffer.

Label construction now uses `snprintf` with the destination capacity. Normal
names and generated RGB labels retain their existing presentation, while an
unusually long name is safely clipped instead of overwriting the stack.

`tests/automated/check-color-selector-label-bounds.py` guards the label paths.
