---
nav_exclude: true
---
# Stepper arithmetic bounds

Stepper range and value resources are `int`, but the old step operation used
`long` for its intermediate.  On platforms where `long` is also 32-bit,
stepping at an extreme value could overflow before the range clamp ran.  The
operation now uses `int64_t`, preserving the existing wrap and clamp behavior
across 32-bit and 64-bit platforms.

`tests/automated/check-stepper-arithmetic.py` keeps the fixed-width arithmetic
in the step path.
