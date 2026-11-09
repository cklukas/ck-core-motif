---
nav_exclude: true
---
# ComboBox popup position bounds

Drop-down ComboBox popup placement now performs its root-coordinate arithmetic
in `int`. The final shell `XmNx`/`XmNy` values are explicitly clamped when
crossing the legacy X11 `Position` boundary, rather than silently wrapping
during the intermediate calculation. This preserves ordinary popup placement
and keeps oversized/high-DPI geometry on screen.

The focused regression is
`tests/automated/check-combobox-popup-position-bounds.py`.
