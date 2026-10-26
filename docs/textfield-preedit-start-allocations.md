---
nav_exclude: true
---
# TextField preedit-start overstrike allocations

Starting an input-method preedit in overstrike mode now validates the snapshot
length and checks both narrow and wide snapshot allocations before copying
text.  An allocation failure leaves the preedit inactive instead of writing
through a null pointer.

Regression coverage: `tests/automated/check-textfield-preedit-start-allocations.py`.
