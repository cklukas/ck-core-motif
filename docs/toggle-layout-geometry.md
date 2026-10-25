---
nav_exclude: true
---
# ToggleButton layout geometry

ToggleButton centers its label and indicator by subtracting margins, insets,
and indicator dimensions from unsigned widget dimensions. Small or temporarily
collapsed buttons could wrap those calculations before converting to a signed
position, moving text or indicators far outside the button.

The layout and indicator drawing paths now convert each dimension before
subtraction. Normal button geometry is preserved while tiny controls remain
stable during resize and realization.
