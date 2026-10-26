---
nav_exclude: true
---
# XmString tag length bounds

The compatibility tag-cache API keeps its historical `int` length parameter,
but sentinel-length calls derive that value from `strlen`.  The cache now
checks the measured `size_t` length against both `INT_MAX` and Xt’s allocation
range before converting it, rejecting unrepresentable tags without wrapping.
The resulting allocation also widens `length + 1` before arithmetic, so the
largest representable legacy length cannot trigger signed overflow.

`tests/automated/check-xmstring-tag-length.py` protects the conversion guard.
