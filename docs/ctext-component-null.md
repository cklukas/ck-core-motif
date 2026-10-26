---
nav_exclude: true
---
# Empty compound-text components

Both compound-text resource encoders now skip the temporary-buffer `memcpy`
when a text component has zero length.  This permits an empty component with no
source pointer without invoking `memcpy` on NULL, while preserving all
non-empty component conversions.

`tests/automated/check-ctext-component-null.py` protects both conversion paths.
