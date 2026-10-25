---
nav_exclude: true
---
# Stepper

`XmStepper` is the compact sibling of `XmNumberBox`. It uses the same bounded
signed-integer range contract, inclusive endpoints, step size, clamping and
optional wrapping, but presents only a value and increment/decrement affordance
for dense layouts. Up/Down and Left/Right keys and pointer activation use the
same typed value-change callback. Existing Motif text and spin widgets are not
modified.

The accessibility provider exposes the committed numeric value and bounds.
`tests/automated/run-stepper.py` verifies explicit sizing, stepping, wrapping,
clamping, callbacks and accessibility from an installed prefix.
