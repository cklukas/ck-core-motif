---
nav_exclude: true
---
# TextField PreeditDraw caret restoration

`PreeditDraw` hides the insertion point while it updates the preedit.  Early
allocation failures in the font-metric and empty-preedit paths now restore the
insertion point before returning, so a transient allocation failure cannot
leave the TextField caret invisible.

Regression coverage: `tests/automated/check-textfield-preedit-draw-caret.py`.
