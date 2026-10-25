---
nav_exclude: true
---
# ColorSelector RGB formatting bounds

ColorSelector derives hexadecimal RGB names into its fixed color-name buffer.
The value is normally short, but its writes should still honor the buffer
contract.

Both slider and parsed-color paths now use `snprintf` with the destination
size.  The generated `#rrggbb` names and label behavior are unchanged.

`tests/automated/check-colors-selector-format-bounds.py` protects both paths.
