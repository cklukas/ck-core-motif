---
nav_exclude: true
---
# Mwm icon placement on small displays

Icon placement derives its grid from display size, icon size, and the optional
placement margin. When those values leave fewer than two slots, spacing is
zero and the grid remains one-dimensional; the spacing calculation no longer
divides by zero while reducing the grid. This keeps one valid placement slot
for small displays and preserves the existing multi-slot spacing behavior.

`tests/automated/check-icon-placement-small-display.py` guards the minimum-slot
and loop conditions.
