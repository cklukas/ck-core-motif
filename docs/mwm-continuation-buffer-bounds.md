# MWM continued configuration-line bounds

MWM accepts backslash-continued configuration lines.  The remaining capacity
for each appended `fgets` call is now calculated as `size_t`, checked for an
exhausted buffer and for `int` representability, and only then passed to the C
library.  Oversized or exhausted continuations stop safely rather than relying
on an implicit pointer-difference conversion.

The focused source contract is covered by
`tests/automated/check-mwm-continuation-buffer-bounds.py`.
