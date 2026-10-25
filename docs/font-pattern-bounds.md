# Font Selector pattern formatting bounds

The Font Selector builds a default XLFD pattern in a fixed `BUFSIZ` buffer.
The encoding resource is inserted into that pattern and may be longer than the
historical built-in values. `SetDisplayedFont` now uses bounded `snprintf`,
preventing an oversized resource from overwriting adjacent state while keeping
the existing pattern for values that fit.

`tests/automated/check-font-pattern-bounds.py` guards the formatting contract.
