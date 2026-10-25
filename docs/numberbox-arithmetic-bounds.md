# NumberBox arithmetic bounds

NumberBox range and step resources are `int`, while the old step operation
used `long` for its intermediate.  On platforms where `long` is 32-bit,
stepping at an extreme value could overflow before the range or wrap clamp
ran.  The operation now uses `int64_t`, preserving behavior across 32-bit and
64-bit platforms.

`tests/automated/check-numberbox-arithmetic.py` keeps the fixed-width
arithmetic in the step path.
