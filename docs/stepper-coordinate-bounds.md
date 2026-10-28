---
nav_exclude: true
---

# Stepper coordinate bounds

Stepper keeps its split geometry in signed `int` arithmetic and clamps the
value and increment/decrement symbol labels before passing them to Motif's
signed `Position` drawing arguments.  Oversized dimensions or font metrics
therefore cannot wrap text to an unrelated screen coordinate.

`tests/automated/check-stepper-coordinate-bounds.py` protects this drawing
contract.
